#include "Engine/SoundSource.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/Outcome/Outcome.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudio/SoundAsset.h"

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

        auto LoadSoundAsset(AZStd::string_view localPath) -> MiniAudio::SoundDataAsset
        {
            auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) };

            auto* const assetManager{ AZ::Data::AssetManager::IsReady()
                                          ? &AZ::Data::AssetManager::Instance()
                                          : nullptr };
            if (!assetManager)
            {
                AZ_Error("SoundSource", false, "Failed to get the asset manager!");
                return {};
            }

            AZ::IO::Path const soundPath{ banksRootPath / localPath };
            MiniAudio::SoundDataAsset soundAsset{ assetManager->GetAsset<MiniAudio::SoundAsset>(
                Internal::FindAssetId(soundPath.c_str()), AZ::Data::AssetLoadBehavior::PreLoad) };

            if (!soundAsset.GetId().IsValid())
            {
                AZ_Error(
                    "SoundSource",
                    false,
                    "Failed to find sound asset with local path: '%s'!",
                    localPath.data());

                return {};
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
                    localPath.data());

                return {};
            }

            return soundAsset;
        };

        auto RegisterSound(
            decltype(MiniAudio::SoundAsset::m_data) assetData, AZStd::string_view name) -> bool
        {
            if (!MiniAudio::MiniAudioInterface::Get())
            {
                AZ_Error("SoundBank", false, "Failed to get mini audio interface.");
                return false;
            }

            ma_engine* const miniAudioEngine{ AudioEngineInterface::Get()->GetSoundEngine() };

            AZStd::string const registerName{ name.data() };
            ma_result result = ma_resource_manager_register_encoded_data(
                ma_engine_get_resource_manager(miniAudioEngine),
                registerName.c_str(),
                assetData.data(),
                assetData.size());

            if (result != MA_SUCCESS)
            {
                AZ_Error(
                    "SoundSource",
                    false,
                    "Failed to register sound '%' with miniaudio!",
                    registerName.c_str());
                return false;
            }

            return true;
        }

    } // namespace Internal

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        SoundSource, "SoundSource", "{5779BE60-D6EE-4A6C-9C1E-5875675A2ED5}");
    AZ_CLASS_ALLOCATOR_IMPL(SoundSource, Audio::AudioImplAllocator);

    void SoundSource::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SoundSource>()->Version(0);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<SoundSource>("SoundSource", "");
            }
        }
    }
    SoundSource::SoundSource(AZ::IO::Path localPath)
        : m_name{ localPath.Native() }
    {
    }

    auto SoundSource::Load() -> AZ::Outcome<void, char const*>
    {
        m_soundAsset = Internal::LoadSoundAsset(m_name.GetCStr());

        if (!m_soundAsset.GetId().IsValid())
        {
            return AZ::Failure("Failed to load sound.");
        }

        auto const nameToRegister{ m_name.GetCStr() };

        if (!Internal::RegisterSound(m_soundAsset->m_data, nameToRegister))
        {
            m_soundAsset.Reset();
            return AZ::Failure("Loaded sound, but failed registration failed.");
        }

        AZLOG_INFO(
            "Registered sound '%s' to miniaudio with the tag '%s'.",
            m_name.GetCStr(),
            nameToRegister);

        return AZ::Success();
    }

} // namespace BopAudio
