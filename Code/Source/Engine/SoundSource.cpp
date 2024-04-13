#include "Engine/SoundSource.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/Outcome/Outcome.h"

#include "BopAudio/Util.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudio/SoundAsset.h"
#include "MiniAudioIncludes.h"

namespace BopAudio
{
    namespace Internal
    {
        auto LoadSoundAsset(AZ::Data::AssetId assetId) -> MiniAudio::SoundDataAsset
        {
            if (!AZ::Data::AssetManager::IsReady())
            {
                AZLOG_ERROR("AssetManager isn't ready!");
                return {};
            }

            auto& assetManager{ AZ::Data::AssetManager::Instance() };

            MiniAudio::SoundDataAsset soundAsset{ assetManager.GetAsset<MiniAudio::SoundAsset>(
                assetId, AZ::Data::AssetLoadBehavior::PreLoad) };

            soundAsset.BlockUntilLoadComplete();

            if (soundAsset.IsError())
            {
                AZ_Error("SoundSource", false, "Failed to find sound asset");
                return {};
            }

            if (!soundAsset.IsReady())
            {
                AZ_Error("SoundBank", false, "Something went wrong loading the sound asset");
                return {};
            }

            return soundAsset;
        };

    } // namespace Internal

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        SoundSource, "SoundSource", "{5779BE60-D6EE-4A6C-9C1E-5875675A2ED5}");

    AZ_CLASS_ALLOCATOR_IMPL(SoundSource, Audio::AudioImplAllocator);

    SoundSource::SoundSource() = default;

    SoundSource::~SoundSource()
    {
        ma_engine* const engine{ SoundEngine::Get()->GetSoundEngine() };
        ma_resource_manager_unregister_data(
            ma_engine_get_resource_manager(engine), m_soundRef.GetCStr());
    }
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

    SoundSource::SoundSource(SoundRef soundResourceRef)
        : m_soundRef{ AZStd::move(soundResourceRef) } {};

    SoundSource::SoundSource(AZ::IO::Path localPath)
        : m_soundRef{ localPath.Native() } {};

    auto SoundSource::RegisterSound() -> bool
    {
        if (!MiniAudio::MiniAudioInterface::Get())
        {
            AZ_Error("SoundBank", false, "Failed to get mini audio interface.");
            return false;
        }

        ma_engine* const miniAudioEngine{ SoundEngine::Get()->GetSoundEngine() };

        ma_result const result = ma_resource_manager_register_encoded_data(
            ma_engine_get_resource_manager(miniAudioEngine),
            m_soundRef.GetCStr(),
            m_soundAsset->m_data.data(),
            m_soundAsset->m_data.size());

        m_registered = (result == MA_SUCCESS);

        AZ_Error(
            "SoundSource",
            m_registered,
            "Failed to register sound '%' with miniaudio!",
            m_soundRef.GetCStr());

        return m_registered;
    }

    auto SoundSource::Load() -> AZ::Outcome<void, char const*>
    {
        auto const path{ AZ::IO::Path{ BanksAlias } / m_soundRef.GetCStr() };

        m_soundAsset = Internal::LoadSoundAsset(
            FindAssetId(path, AZ::AzTypeInfo<MiniAudio::SoundAsset>::Uuid()));

        if (!m_soundAsset.IsReady())
        {
            return AZ::Failure("Failed to load sound.");
        }

        if (!RegisterSound())
        {
            m_soundAsset.Reset();
            return AZ::Failure("Loaded sound, but failed registration failed.");
        }

        AZLOG_INFO("Registered sound '%s' to miniaudio", m_soundRef.GetCStr());
        return AZ::Success();
    }

} // namespace BopAudio
