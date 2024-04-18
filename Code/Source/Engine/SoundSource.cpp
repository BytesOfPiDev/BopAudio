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
#include <AzCore/Debug/Trace.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Serialization/EditContextConstants.inl>

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

    AZ_CLASS_ALLOCATOR_IMPL(SoundSource, AZ::SystemAllocator);

    void SoundSource::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SoundSource>()
                ->Version(0)
                ->Field("SoundRef", &SoundSource::m_soundRef)
                ->Field("SoundAsset", &SoundSource::m_soundAsset);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<SoundSource>("SoundSource", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &SoundSource::m_soundAsset,
                        "Sound Asset",
                        "")
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify, &SoundSource::OnSoundAssetChanged);
            }
        }
    }

    SoundSource::SoundSource() = default;

    SoundSource::~SoundSource()
    {
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

        AZ_Warning("SoundSource", m_registered, "RegisterSound is unimplemented.");

        return m_registered;
    }

    void SoundSource::OnSoundAssetChanged()
    {
        AZ_Info("SoundSource", "OnSoundAssetChanged");
    }

    auto SoundSource::Load() -> AZ::Outcome<void, char const*>
    {
        AZ_Warning("SoundSource", false, "Load is unimplemented.");
        return AZ::Success();
    }

} // namespace BopAudio
