#include "BopAudioSystemComponent.h"

#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "ScriptCanvas/Nodes/AudioControlNode.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId);

    void BopAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* const serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BopAudioSystemComponent, AZ::Component>()->Version(0);
        }
    }

    void BopAudioSystemComponent::GetProvidedServices(
        AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioService"));
    }

    void BopAudioSystemComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioService"));
    }

    void BopAudioSystemComponent::GetRequiredServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("AssetDatabaseService"));
        required.push_back(AZ_CRC_CE("AssetCatalogService"));
    }

    void BopAudioSystemComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("MiniAudioService"));
    }

    BopAudioSystemComponent::BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == nullptr)
        {
            BopAudioInterface::Register(this);
        }
    }

    BopAudioSystemComponent::~BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == this)
        {
            BopAudioInterface::Unregister(this);
        }
    }

    void BopAudioSystemComponent::Init()
    {
        AZStd::make_unique<Nodes::AudioControlNode>();
    }

    void BopAudioSystemComponent::Activate()
    {
        BopAudioRequestBus::Handler::BusConnect();
    }

    void BopAudioSystemComponent::Deactivate()
    {
        BopAudioRequestBus::Handler::BusDisconnect();
    }

} // namespace BopAudio
