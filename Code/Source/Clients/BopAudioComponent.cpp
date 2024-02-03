#include "BopAudioComponent.h"

#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/EditContextConstants.inl"
#include "AzCore/Serialization/SerializeContext.h"
#include "BopAudio/BopAudioComponentBus.h"

namespace BopAudio
{
    void BopAudioComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BopAudioComponent, AZ::Component>()->Version(1)->Field(
                "File", &BopAudioComponent::m_audioAsset);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<BopAudioComponent>("Steam Audio", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void BopAudioComponent::GetProvidedServices(
        AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioComponentService"));
    }

    void BopAudioComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioComponentService"));
    }

    void BopAudioComponent::GetRequiredServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void BopAudioComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void BopAudioComponent::Init()
    {
    }

    void BopAudioComponent::Activate()
    {
        BopAudioComponentRequestBus::Handler::BusConnect(GetEntityId());
    }

    void BopAudioComponent::Deactivate()
    {
        BopAudioComponentRequestBus::Handler::BusDisconnect();
    }
} // namespace BopAudio
