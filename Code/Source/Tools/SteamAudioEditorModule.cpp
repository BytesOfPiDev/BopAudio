
#include "SteamAudioEditorSystemComponent.h"
#include <SteamAudio/SteamAudioTypeIds.h>
#include <SteamAudioModuleInterface.h>

void InitSteamAudioResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(SteamAudio);
}

namespace SteamAudio
{
    class SteamAudioEditorModule : public SteamAudioModuleInterface
    {
    public:
        AZ_RTTI(SteamAudioEditorModule, SteamAudioEditorModuleTypeId, SteamAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(SteamAudioEditorModule, AZ::SystemAllocator);

        SteamAudioEditorModule()
        {
            InitSteamAudioResources();

            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and
            // EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(
                m_descriptors.end(),
                {
                    SteamAudioEditorSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<SteamAudioEditorSystemComponent>(),
            };
        }
    };
} // namespace SteamAudio

AZ_DECLARE_MODULE_CLASS(Gem_SteamAudio, SteamAudio::SteamAudioEditorModule)
