#include "Engine/SoundBank.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "MiniAudio/SoundAsset.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudioIncludes.h"

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

            soundNames.emplace_back(object.value.GetString());

            AZLOG_INFO("BopAudio: Found sound '%s'.\n", object.name.GetString());
        }

        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) };

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

                MiniAudio::SoundDataAsset soundAsset{ assetManager->GetAsset<MiniAudio::SoundAsset>(
                    Internal::FindAssetId(banksRootPath / soundName.GetStringView()), AZ::Data::AssetLoadBehavior::PreLoad) };

                if (!soundAsset.GetId().IsValid())
                {
                    AZ_Error("SoundBank", false, "Failed to find sound asset '%s'!", soundName.GetCStr());
                    return false;
                }

                soundAsset.QueueLoad();
                soundAsset.BlockUntilLoadComplete();

                auto const& assetData{ soundAsset->m_data };
                if (assetData.empty())
                {
                    AZ_Error("SoundBank", false, "Failed to load sound asset '%s'!", soundName.GetCStr());
                    return false;
                }

                if (!AudioEngineInterface::Get())
                {
                    AZ_Error("SoundBank", false, "Failed to get mini audio engine.");
                    return false;
                }

                ma_engine* const miniAudioEngine{ AudioEngineInterface::Get()->GetSoundEngine() };

                ma_result result = ma_resource_manager_register_encoded_data(
                    ma_engine_get_resource_manager(miniAudioEngine), soundName.GetCStr(), assetData.data(), assetData.size());

                if (result != MA_SUCCESS)
                {
                    AZ_Error("SoundBank", false, "Failed to register sound data '%s' with miniaudio!", soundName.GetCStr());

                    fileImplData->m_soundAssets.erase(soundName);
                    return false;
                }

                m_soundAssets[soundName] = soundAsset;
                fileImplData->m_soundNames.insert(soundName);

                return true;
            });

        return isSuccess;
    }
} // namespace BopAudio
