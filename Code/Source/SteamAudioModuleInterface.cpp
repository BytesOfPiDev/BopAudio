
#include "SteamAudioModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <SteamAudio/SteamAudioTypeIds.h>

#include <Clients/SteamAudioSystemComponent.h>

namespace SteamAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(SteamAudioModuleInterface,
        "SteamAudioModuleInterface", SteamAudioModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(SteamAudioModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(SteamAudioModuleInterface, AZ::SystemAllocator);

    SteamAudioModuleInterface::SteamAudioModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            SteamAudioSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList SteamAudioModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<SteamAudioSystemComponent>(),
        };
    }
} // namespace SteamAudio
