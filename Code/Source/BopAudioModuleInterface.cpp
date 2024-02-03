
#include "BopAudioModuleInterface.h"

#include "AzCore/Memory/Memory.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/BopAudioComponent.h"
#include "Clients/BopAudioSystemComponent.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        BopAudioModuleInterface,
        "BopAudioModuleInterface",
        BopAudioModuleInterfaceTypeId); // NOLINT
    AZ_RTTI_NO_TYPE_INFO_IMPL(BopAudioModuleInterface, AZ::Module); // NOLINT
    AZ_CLASS_ALLOCATOR_IMPL(BopAudioModuleInterface, AZ::SystemAllocator); // NOLINT

    BopAudioModuleInterface::BopAudioModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors
        // here. Add ALL components descriptors associated with this gem to
        // m_descriptors. This will associate the AzTypeInfo information for the
        // components with the the SerializeContext, BehaviorContext and
        // EditContext. This happens through the [MyComponent]::Reflect()
        // function.
        m_descriptors.insert(
            m_descriptors.end(),
            { BopAudioSystemComponent::CreateDescriptor(), BopAudioComponent::CreateDescriptor() });
    }

    AZ::ComponentTypeList BopAudioModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<BopAudioSystemComponent>(),
        };
    }
} // namespace BopAudio
