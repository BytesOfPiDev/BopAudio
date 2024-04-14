#include "IGem.h"
#include "Scene/Behaviors/AudioEventGroupBehavior.h"
#include "Scene/Processors/AudioEventProcessor.h"
#include <AzCore/Module/DynamicModuleHandle.h>

namespace BopAudio
{
    class BopAudioSceneModule : public CryHooksModule
    {
    public:
        AZ_RTTI(BopAudioSceneModule, "{6D7869D9-FABC-47A3-BE1F-CCE3A5A18CE6}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(BopAudioSceneModule, AZ::SystemAllocator);

        BopAudioSceneModule()
        {
            // The SceneAPI libraries require specialized initialization. As early as possible, be
            // sure to repeat the following two lines for any SceneAPI you want to use. Omitting
            // these calls or making them too late can cause problems such as missing EBus events.
            m_sceneCoreModule = AZ::DynamicModuleHandle::Create("SceneCore");
            m_sceneCoreModule->Load(true);

            m_descriptors.insert(
                m_descriptors.end(),
                { BopAudioScene::AudioEventGroupBehavior::CreateDescriptor(),
                  BopAudioScene::LoadingTrackingProcessor::CreateDescriptor() });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        [[nodiscard]] auto GetRequiredSystemComponents() const -> AZ::ComponentTypeList override
        {
            return AZ::ComponentTypeList{};
        }

    private:
        AZStd::unique_ptr<AZ::DynamicModuleHandle> m_sceneCoreModule;
    };
} // namespace BopAudio

AZ_DECLARE_MODULE_CLASS(Gem_BopAudio_Scene, BopAudio::BopAudioSceneModule)
