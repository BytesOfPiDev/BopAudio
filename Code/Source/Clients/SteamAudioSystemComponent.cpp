#include "SteamAudioSystemComponent.h"

#include "AzCore/Serialization/SerializeContext.h"

#include "Clients/AudioAssetHandler.h"
#include "SteamAudio/SoundAsset.h"
#include "SteamAudio/SteamAudioTypeIds.h"
#include "phonon.h"

namespace SteamAudio
{
    AZ_COMPONENT_IMPL(SteamAudioSystemComponent, "SteamAudioSystemComponent", SteamAudioSystemComponentTypeId);

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
    }

    void SteamAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SteamAudioService"));
    }

    void SteamAudioSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void SteamAudioSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
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
        m_contextSettings.version = STEAMAUDIO_VERSION;

        iplContextCreate(&m_contextSettings, &m_context);

        m_hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

        m_audioSettings.samplingRate = 44100;
        m_audioSettings.frameSize = 1024;

        iplHRTFCreate(m_context, &m_audioSettings, &m_hrtfSettings, &m_hrtf);

        SteamAudioRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();

        auto* handler = aznew SteamAudioAssetHandler();
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);
        m_assetHandlers.emplace_back(handler);
    }

    void SteamAudioSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SteamAudioRequestBus::Handler::BusDisconnect();

        iplHRTFRelease(&m_hrtf);
        iplContextRelease(&m_context);
    }

    void SteamAudioSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace SteamAudio
