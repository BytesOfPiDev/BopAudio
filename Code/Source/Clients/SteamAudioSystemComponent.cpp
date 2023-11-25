
#include "SteamAudioSystemComponent.h"

#include <SteamAudio/SteamAudioTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace SteamAudio
{
    AZ_COMPONENT_IMPL(SteamAudioSystemComponent, "SteamAudioSystemComponent",
        SteamAudioSystemComponentTypeId);

    void SteamAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SteamAudioSystemComponent, AZ::Component>()
                ->Version(0)
                ;
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
        SteamAudioRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void SteamAudioSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SteamAudioRequestBus::Handler::BusDisconnect();
    }

    void SteamAudioSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace SteamAudio
