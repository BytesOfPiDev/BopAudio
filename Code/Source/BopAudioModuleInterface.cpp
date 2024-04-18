
#include "BopAudioModuleInterface.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/BopAudioSystemComponent.h"

/////////////////////////////////////////////////////////////
// This registration only needs to happen once per module
// You can keep it here, or move it into this module's
// system component
#include <ScriptCanvas/AutoGen/ScriptCanvasAutoGenRegistry.h>

/////////////////////////////////////////////////////////////

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        BopAudioModuleInterface, "BopAudioModuleInterface", BopAudioModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(BopAudioModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(BopAudioModuleInterface, AZ::SystemAllocator);

    BopAudioModuleInterface::BopAudioModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors
        // here. Add ALL components descriptors associated with this gem to
        // m_descriptors. This will associate the AzTypeInfo information for the
        // components with the the SerializeContext, BehaviorContext and
        // EditContext. This happens through the [MyComponent]::Reflect()
        // function.
        m_descriptors.insert(m_descriptors.end(), { BopAudioSystemComponent::CreateDescriptor() });
    }

    auto BopAudioModuleInterface::GetRequiredSystemComponents() const -> AZ::ComponentTypeList
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<BopAudioSystemComponent>(),
        };
    }
} // namespace BopAudio
