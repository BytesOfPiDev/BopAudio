
#include "BopAudioEditorSystemComponent.h"
#include <BopAudio/BopAudioTypeIds.h>
#include <BopAudioModuleInterface.h>

void InitBopAudioResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(BopAudio);
}

namespace BopAudio
{
    class BopAudioEditorModule : public BopAudioModuleInterface
    {
    public:
        AZ_RTTI(BopAudioEditorModule, BopAudioEditorModuleTypeId, BopAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(BopAudioEditorModule, AZ::SystemAllocator);

        BopAudioEditorModule()
        {
            InitBopAudioResources();

            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and
            // EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(
                m_descriptors.end(),
                {
                    BopAudioEditorSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<BopAudioEditorSystemComponent>(),
            };
        }
    };
} // namespace BopAudio

AZ_DECLARE_MODULE_CLASS(Gem_BopAudio, BopAudio::BopAudioEditorModule)