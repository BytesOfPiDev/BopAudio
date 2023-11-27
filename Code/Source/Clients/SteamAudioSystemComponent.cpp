#include "SteamAudioSystemComponent.h"

#include "AzCore/Console/ILogger.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Settings/SettingsRegistry.h"
#include "AzFramework/Platform/PlatformDefaults.h"

#include "Clients/AudioAssetHandler.h"
#include "Engine/AudioSystemImpl_SteamAudio.h"
#include "SteamAudio/AudioAsset.h"
#include "SteamAudio/SteamAudioTypeIds.h"
#include "phonon.h"

namespace SteamAudio
{
    AZ_COMPONENT_IMPL(SteamAudioSystemComponent, "SteamAudioSystemComponent", SteamAudioSystemComponentTypeId); // NOLINT

    void SteamAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        AudioAsset::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SteamAudioSystemComponent, AZ::Component>()->Version(0);
        }
    }

    void SteamAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("SteamAudioService"));
        provided.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void SteamAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SteamAudioService"));
        incompatible.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void SteamAudioSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("AudioSystemService"));
    }

    void SteamAudioSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC("AudioSystemService"));
    }

    SteamAudioSystemComponent::SteamAudioSystemComponent()
    {
        if (SteamAudioInterface::Get() == nullptr)
        {
            SteamAudioInterface::Register(this);
        }
    }

    SteamAudioSystemComponent::~SteamAudioSystemComponent()
    {
        if (SteamAudioInterface::Get() == this)
        {
            SteamAudioInterface::Unregister(this);
        }
    }

    void SteamAudioSystemComponent::Init()
    {
    }

    void SteamAudioSystemComponent::Activate()
    {
        SteamAudioRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        Audio::Gem::EngineRequestBus::Handler::BusConnect();

        /*
              AZ::Data::AssetCatalogRequestBus::Broadcast(
                  &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        */
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);

        m_assetHandlers.emplace_back(aznew SteamAudioAssetHandler()); // NOLINT
    }

    void SteamAudioSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        Audio::Gem::EngineRequestBus::Handler::BusDisconnect();
        SteamAudioRequestBus::Handler::BusDisconnect();
    }

    void SteamAudioSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

    auto SteamAudioSystemComponent::Initialize() -> bool
    {
        AZ::SettingsRegistryInterface::FixedValueString assetPlatform =
            AzFramework::OSPlatformToDefaultAssetPlatform(AZ_TRAIT_OS_PLATFORM_CODENAME);

        m_engineSteamAudio = AZStd::make_unique<SteamAudio::AudioSystemImpl_SteamAudio>(assetPlatform.c_str());
        if (m_engineSteamAudio)
        {
            Audio::SystemRequest::Initialize initRequest;
            AZ::Interface<Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initRequest));

            return true;
            AZLOG_INFO("AudioEngineSteamAudio created!"); // NOLINT
        }
        else
        {
            AZLOG_ERROR("Could not create AudioEngineSteamAudio!"); // NOLINT
        }
        return false;
    }

    void SteamAudioSystemComponent::Release()
    {
        m_engineSteamAudio = nullptr;
    }

} // namespace SteamAudio
