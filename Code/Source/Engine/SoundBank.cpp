#include "Engine/SoundBank.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudio/SoundAsset.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "MiniAudioIncludes.h"
#include <AzCore/IO/FileIO.h>

namespace BopAudio
{
    namespace Internal
    {
        auto FindAssetId(AZ::IO::Path const& fullPath) -> AZ::Data::AssetId
        {
            AZ::Data::AssetId resultAssetId{};
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                resultAssetId,
                &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
                fullPath.c_str(),
                AZ::AzTypeInfo<MiniAudio::SoundAsset>::Uuid(),
                true);

            return resultAssetId;
        }
    } // namespace Internal

    SoundBank::SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData)
        : m_fileEntryInfo(fileEntryData)
    {
    }

    auto SoundBank::Load() -> bool
    {
        /*
              rapidjson::Document doc;
              AZStd::span<char> buffer(static_cast<char*>(m_fileEntryInfo->pFileData), m_fileEntryInfo->nSize);
              doc.Parse(buffer.data());

              auto* const soundsObjectJsonPtr{ rapidjson::GetValueByPointer(doc, JsonKeys::SoundFileNames) };
              if (!soundsObjectJsonPtr)
              {
                  AZ_Warning("SoundBank", false, "SoundBank '%s' is empty.", m_fileEntryInfo->sFileName);
                  return false;
              }

              AZStd::vector<AZ::Name> soundNames{};
              for (auto& object : soundsObjectJsonPtr->GetObject())
              {
                  if (!object.name.IsString())
                  {
                      AZ_Error("SoundBank", false, "Unexpected value type for SoundBank '%s'. Expected a string.", object.name.GetString());
                      continue;
                  }

                  auto& soundName{ soundNames.emplace_back(object.name.GetString()) };

                  AZLOG_INFO("BopAudio: Found sound '%s'.\n", soundName.GetCStr());
              }
        */

        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) };

        auto soundNames{ GetSoundNamesFromSoundBankFile(banksRootPath) };

        auto* fileImplData{ static_cast<SATLAudioFileEntryData_BopAudio*>(m_fileEntryInfo->pImplData) };

        AZLOG_INFO("BopAudio: Loading sounds...");
        bool const isSuccess = AZStd::ranges::all_of(
            soundNames,
            [&banksRootPath, &fileImplData, this](AZ::Name const& soundName) -> bool
            {
                AZLOG_INFO("BopAudio: Loading sound.");

                auto* const assetManager{ AZ::Data::AssetManager::IsReady() ? &AZ::Data::AssetManager::Instance() : nullptr };
                if (!assetManager)
                {
                    AZ_Error("SoundBank", false, "Failed to get the asset manager!");
                    return false;
                }
                AZ::IO::Path const soundPath{ banksRootPath / soundName.GetStringView() };

                MiniAudio::SoundDataAsset soundAsset{ assetManager->GetAsset<MiniAudio::SoundAsset>(
                    Internal::FindAssetId(soundPath.c_str()), AZ::Data::AssetLoadBehavior::PreLoad) };

                if (!soundAsset.GetId().IsValid())
                {
                    AZ_Error(
                        "SoundBank", false, "Failed to find sound asset '%s' using path '%s'!", soundName.GetCStr(), soundPath.c_str());
                    return false;
                }

                soundAsset.QueueLoad();
                soundAsset.BlockUntilLoadComplete();

                auto const& assetData{ soundAsset->m_data };
                if (assetData.empty())
                {
                    AZ_Error(
                        "SoundBank", false, "Failed to load sound asset '%s' using path '%s'!", soundName.GetCStr(), soundPath.c_str());
                    return false;
                }

                if (!MiniAudio::MiniAudioInterface::Get())
                {
                    AZ_Error("SoundBank", false, "Failed to get mini audio interface.");
                    return false;
                }

                ma_engine* const miniAudioEngine{ MiniAudio::MiniAudioInterface::Get()->GetSoundEngine() };

                ma_result result = ma_resource_manager_register_encoded_data(
                    ma_engine_get_resource_manager(miniAudioEngine), soundAsset.GetHint().c_str(), assetData.data(), assetData.size());

                if (result != MA_SUCCESS)
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to register sound data '%s' with miniaudio! Tried registering with '%s'.",
                        soundName.GetCStr(),
                        soundAsset.GetHint().c_str());

                    fileImplData->m_soundAssets.erase(soundName);
                    return false;
                }

                AZLOG_INFO(
                    "SoundBank | Registered sound '%s' to miniaudio with the tag '%s'.", soundName.GetCStr(), soundAsset.GetHint().c_str());

                m_soundAssets[soundName] = soundAsset;
                fileImplData->m_soundNames.insert(soundName);

                return true;
            });

        return isSuccess;
    }

    auto SoundBank::CreateSound(AZ::Name const& soundName) -> SoundPtr
    {
        auto const soundAsset{ GetSoundAsset(soundName) };
        if (!soundAsset.GetId().IsValid())
        {
            return nullptr;
        }
        ma_engine* engine{ MiniAudio::MiniAudioInterface::Get()->GetSoundEngine() };
        auto soundPtr{ MakeSoundPtr(soundName, engine) };

        ma_uint32 const flags = MA_SOUND_FLAG_DECODE;
        ma_result result = ma_sound_init_from_file(engine, soundAsset.GetHint().c_str(), flags, nullptr, nullptr, soundPtr.get());
        if (result != MA_SUCCESS)
        {
            AZ_Error("SoundBank", false, "Failed to init sound! Sound: %s. Hint: %s.", soundName.GetCStr(), soundAsset.GetHint().c_str());

            return nullptr;
        }

        ma_sound_set_volume(soundPtr.get(), 1.0f);
        ma_sound_set_looping(soundPtr.get(), true);

        return AZStd::move(soundPtr);
    }

    auto GetSoundNamesFromSoundBankFile(AZ::IO::Path soundBankFilePath) -> SoundNames
    {
        rapidjson::Document doc;
        AZStd::vector<char> buffer = [&soundBankFilePath]() -> decltype(buffer)
        {
            AZ::IO::FileIOStream fileStream{ soundBankFilePath.c_str(), AZ::IO::OpenMode::ModeRead };
            decltype(buffer) tempBuffer(fileStream.GetLength());
            fileStream.Read(tempBuffer.size(), tempBuffer.data());

            return tempBuffer;
        }();

        doc.Parse(buffer.data(), buffer.size());

        auto* const soundsObjectJsonPtr{ rapidjson::GetValueByPointer(doc, JsonKeys::SoundFileNames) };

        if (!soundsObjectJsonPtr)
        {
            AZLOG_WARN("SoundBank '%s' is empty.", soundBankFilePath.c_str());
            return {};
        }

        SoundNames soundNames{};

        for (auto& object : soundsObjectJsonPtr->GetObject())
        {
            if (!object.name.IsString())
            {
                AZ_Error(
                    "GetSoundNamesFromSoundBankFile",
                    false,
                    "Unexpected value type for SoundBank '%s'. Expected a string.",
                    object.name.GetString());
                continue;
            }
            soundNames.emplace_back(object.name.GetString());
        }

        return soundNames;
    }
} // namespace BopAudio
