#include "Engine/SoundBank.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/Sound.h"
#include "IAudioSystem.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudio/SoundAsset.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "MiniAudioIncludes.h"
#include <AzCore/PlatformId/PlatformDefaults.h>

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

        auto MakeSoundBankId(AZStd::string_view fileName) -> AZ::Name
        {
            auto const soundBankPath =
                fileName.empty() ? nullptr : AZ::IO::Path{ GetBanksRootPath() } / fileName;
            return soundBankPath.empty() ? AZ::Name{} : BA_SoundBankId{ soundBankPath.Native() };
        }
    } // namespace Internal

    SoundBank::SoundBank(AZStd::string_view soundBankFileName)
        : m_id{ Internal::MakeSoundBankId(soundBankFileName) }
        , m_soundBankName{ soundBankFileName }
    {
    }

    SoundBank::SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData)
        : m_id{ Internal::MakeSoundBankId(fileEntryData ? fileEntryData->sFileName : nullptr) }
        , m_fileEntryInfo{ fileEntryData }
        , m_soundBankName{ fileEntryData ? decltype(m_soundBankName){ fileEntryData->sFileName }
                                         : decltype(m_soundBankName){} }
    {
    }

    auto SoundBank::LoadInitBank() -> SoundBank
    {
        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) / InitBank };

        auto buffer{ LoadSoundBankToBuffer(banksRootPath.Native()) };

        SoundBank initBank{ InitBank };
        initBank.Load();

        return initBank;
    }

    auto SoundBank::Load(AZStd::span<char> soundBankFileBuffer)
    {
        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) };

        auto soundNames{ GetSoundNamesFromSoundBankFile(soundBankFileBuffer) };

        AZLOG_INFO("BopAudio: Loading sounds...");
        bool const isSuccess = AZStd::ranges::all_of(
            soundNames,
            [&banksRootPath, this](AZ::Name const& soundName) -> bool
            {
                AZLOG_INFO("BopAudio: Loading sound.");

                auto* const assetManager{ AZ::Data::AssetManager::IsReady()
                                              ? &AZ::Data::AssetManager::Instance()
                                              : nullptr };
                if (!assetManager)
                {
                    AZ_Error("SoundBank", false, "Failed to get the asset manager!");
                    return false;
                }
                AZ::IO::Path const soundPath{ banksRootPath / soundName.GetStringView() };

                MiniAudio::SoundDataAsset soundAsset{ assetManager->GetAsset<MiniAudio::SoundAsset>(
                    Internal::FindAssetId(soundPath.c_str()),
                    AZ::Data::AssetLoadBehavior::PreLoad) };

                if (!soundAsset.GetId().IsValid())
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to find sound asset '%s' using path '%s'!",
                        soundName.GetCStr(),
                        soundPath.c_str());
                    return false;
                }

                soundAsset.QueueLoad();
                soundAsset.BlockUntilLoadComplete();

                auto const& assetData{ soundAsset->m_data };
                if (assetData.empty())
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to load sound asset '%s' using path '%s'!",
                        soundName.GetCStr(),
                        soundPath.c_str());
                    return false;
                }

                if (!MiniAudio::MiniAudioInterface::Get())
                {
                    AZ_Error("SoundBank", false, "Failed to get mini audio interface.");
                    return false;
                }

                ma_engine* const miniAudioEngine{ AudioEngineInterface::Get()->GetSoundEngine() };

                AZStd::string const registerName{ soundAsset.GetHint().c_str() };
                ma_result result = ma_resource_manager_register_encoded_data(
                    ma_engine_get_resource_manager(miniAudioEngine),
                    registerName.c_str(),
                    assetData.data(),
                    assetData.size());

                if (result != MA_SUCCESS)
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to register sound '%' with miniaudio!",
                        registerName.c_str());
                    return false;
                }
                AZLOG_INFO("Sound registered with: '%s'.", soundAsset.GetHint().c_str());

                auto const idName{ NumericIdToName(BA_UniqueId{ soundName.GetStringView() }) };
                result = ma_resource_manager_register_encoded_data(
                    ma_engine_get_resource_manager(miniAudioEngine),
                    idName.GetCStr(),
                    assetData.data(),
                    assetData.size());

                if (result != MA_SUCCESS)
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to register sound '%' with miniaudio!",
                        idName.GetCStr());

                    return false;
                }

                AZLOG_INFO(
                    "SoundBank | Registered sound '%s' to miniaudio with the "
                    "tag '%s'.",
                    soundName.GetCStr(),
                    soundAsset.GetHint().c_str());

                m_soundAssets[soundName] = soundAsset;

                return true;
            });

        return isSuccess;
    }

    auto SoundBank::Load() -> bool
    {
        bool const loadedSuccessfully = [this]() -> decltype(loadedSuccessfully)
        {
            AZStd::span<char> buffer = m_fileEntryInfo
                ? decltype(buffer){ static_cast<char*>(m_fileEntryInfo->pFileData),
                                    m_fileEntryInfo->nSize }
                : decltype(buffer){};

            // Try to load with file entry info first.
            if (Load(buffer))
            {
                return true;
            }

            // If that didn't work, try using the name of the sound bank.
            auto loadedBuffer{ LoadSoundBankToBuffer(
                AZ::IO::Path{ GetBanksRootPath() } / m_soundBankName.GetCStr()) };

            return Load(loadedBuffer);
        }();

        if (!loadedSuccessfully)
        {
            return false;
        };

        auto* fileImplData{ static_cast<SATLAudioFileEntryData_BopAudio*>(
            m_fileEntryInfo->pImplData) };
        AZStd::ranges::for_each(
            m_soundAssets,
            [fileImplData](auto const& soundNameAssetPair)
            {
                auto const& [soundName, soundAsset]{ soundNameAssetPair };
                fileImplData->m_soundNames.insert(soundName);
            });

        return true;
    }

    auto SoundBank::GetSoundAsset(AZ::Name const& soundName) const -> MiniAudio::SoundDataAsset
    {
        auto iter{ m_soundAssets.find(soundName) };
        return (iter != AZStd::end(m_soundAssets)) ? iter->second : MiniAudio::SoundDataAsset{};
    };

    auto SoundBank::CreateSound(BA_UniqueId soundId) const -> SoundPtr
    {
        auto const iter{ AZStd::ranges::find_if(
            m_soundAssets,
            [&soundId](auto const& soundNameAssetPair) -> bool
            {
                auto const& [soundName, soundAsset]{ soundNameAssetPair };
                return soundId == BA_UniqueId{ soundName.GetCStr() };
            }) };

        auto const& [soundName, soundNameAssetPair]{ *iter };

        return CreateSoundByName(soundName);
    }

    /*
        auto SoundBank::CreateSound(AZ::Name const& soundName) const -> SoundPtr
        {
            auto const soundAsset{ GetSoundAsset(soundName) };
            if (!soundAsset.GetId().IsValid())
            {
                return nullptr;
            }
            ma_engine* engine{
       MiniAudio::MiniAudioInterface::Get()->GetSoundEngine() }; auto soundPtr{
       MakeSoundPtr(soundName) };

            ma_uint32 const flags = MA_SOUND_FLAG_DECODE;
            ma_result result = ma_sound_init_from_file(engine,
       soundAsset.GetHint().c_str(), flags, nullptr, nullptr, soundPtr.get());
            if (result != MA_SUCCESS)
            {
                AZ_Error("SoundBank", false, "Failed to init sound! Sound: %s.
       Hint: %s.", soundName.GetCStr(), soundAsset.GetHint().c_str());

                return nullptr;
            }

            ma_sound_set_volume(soundPtr.get(), 1.0f);
            ma_sound_set_looping(soundPtr.get(), true);

            return AZStd::move(soundPtr);
        }

        auto SoundBank::CreateSound(decltype(m_soundAssets)::const_iterator
       soundIter) const -> SoundPtr
        {
            if (soundIter == AZStd::end(m_soundAssets))
            {
                return nullptr;
            }
            auto const& [soundName, soundAsset]{ *soundIter };

            ma_engine* engine{ AudioEngineInterface::Get()->GetSoundEngine() };
            auto soundPtr{ MakeSoundPtr(soundName) };

            ma_uint32 const flags = MA_SOUND_FLAG_DECODE;
            ma_result result = ma_sound_init_from_file(engine,
       soundAsset.GetHint().c_str(), flags, nullptr, nullptr, soundPtr.get());
            if (result != MA_SUCCESS)
            {
                AZ_Error("SoundBank", false, "Failed to init sound! Sound: %s.
       Hint: %s.", soundName.GetCStr(), soundAsset.GetHint().c_str());

                return nullptr;
            }

            ma_sound_set_volume(soundPtr.get(), 1.0f);
            ma_sound_set_looping(soundPtr.get(), false);

            return AZStd::move(soundPtr);
        }
    */

    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath) -> AZStd::vector<char>
    {
        using ReturnType = decltype(LoadSoundBankToBuffer(soundBankFilePath));

        return [&soundBankFilePath]() -> ReturnType
        {
            AZ::IO::FileIOStream fileStream{ soundBankFilePath.c_str(),
                                             AZ::IO::OpenMode::ModeRead };
            ReturnType tempBuffer(fileStream.GetLength());
            fileStream.Read(tempBuffer.size(), tempBuffer.data());

            return tempBuffer;
        }();
    }

    auto GetSoundNamesFromSoundBankFile(AZStd::span<char> soundBankFileBuffer) -> SoundNames
    {
        rapidjson::Document doc;

        doc.Parse(soundBankFileBuffer.data(), soundBankFileBuffer.size());

        auto* const soundsObjectJsonPtr{ rapidjson::GetValueByPointer(
            doc, JsonKeys::SoundFileNames) };

        if (!soundsObjectJsonPtr)
        {
            AZLOG_WARN("The SoundBank buffer is empty.");
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
                    "Unexpected value type for SoundBank '%s'. Expected a "
                    "string.",
                    object.name.GetString());
                continue;
            }
            soundNames.emplace_back(object.name.GetString());
        }

        return soundNames;
    }

    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames
    {
        AZStd::vector<char> buffer = LoadSoundBankToBuffer(soundBankFilePath);
        return GetSoundNamesFromSoundBankFile(buffer);
    }

    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames
    {
        return GetSoundNamesFromSoundBankFile(
            AZ::IO::Path{ DefaultBanksPath } / soundBankName.GetStringView());
    }

} // namespace BopAudio
