#include "SteamAudioComponent.h"

#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/EditContextConstants.inl"
#include "AzCore/Serialization/SerializeContext.h"
#include "SteamAudio/SteamAudioComponentBus.h"

namespace SteamAudio
{
    void SteamAudioComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SteamAudioComponent, AZ::Component>()->Version(1)->Field("File", &SteamAudioComponent::m_audioAsset);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<SteamAudioComponent>("Steam Audio", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "SteamAudio")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void SteamAudioComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("SteamAudioComponentService"));
    }

    void SteamAudioComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SteamAudioComponentService"));
    }

    void SteamAudioComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void SteamAudioComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void SteamAudioComponent::Init()
    {
    }

    void SteamAudioComponent::Activate()
    {
        SteamAudioComponentRequestBus::Handler::BusConnect(GetEntityId());
    }

    void SteamAudioComponent::Deactivate()
    {
        SteamAudioComponentRequestBus::Handler::BusDisconnect();
    }
} // namespace SteamAudio
