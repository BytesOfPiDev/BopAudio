#pragma once

#include <SceneAPI/SceneCore/Components/BehaviorComponent.h>
#include <SceneAPI/SceneCore/Events/AssetImportRequest.h>
#include <SceneAPI/SceneCore/Events/ManifestMetaInfoBus.h>

namespace BopAudioScene
{
    // The LoggingGroupBehavior shows how a behavior can be written that monitors
    // manifest activity and reacts to it in order to setup default values for
    // manifest entries. It also demonstrates how to register new UI elements with
    // the SceneAPI.
    class AudioEventGroupBehavior
        : public AZ::SceneAPI::SceneCore::BehaviorComponent
        , public AZ::SceneAPI::Events::ManifestMetaInfoBus::Handler
        , public AZ::SceneAPI::Events::AssetImportRequestBus::Handler
    {
    public:
        AZ_COMPONENT(
            AudioEventGroupBehavior,
            "{893BBBC2-D29C-4C38-8C4D-BE45703BC82B}",
            AZ::SceneAPI::SceneCore::BehaviorComponent);
        AZ_DISABLE_COPY_MOVE(AudioEventGroupBehavior);

        AudioEventGroupBehavior() = default;
        ~AudioEventGroupBehavior() override = default;

        void Activate() override;
        void Deactivate() override;
        static void Reflect(AZ::ReflectContext* context);

        void GetCategoryAssignments(
            CategoryRegistrationList& categories,
            const AZ::SceneAPI::Containers::Scene& scene) override;
        auto UpdateManifest(
            AZ::SceneAPI::Containers::Scene& scene,
            ManifestAction action,
            RequestingApplication requester) -> AZ::SceneAPI::Events::ProcessingResult override;
        void InitializeObject(
            const AZ::SceneAPI::Containers::Scene& scene,
            AZ::SceneAPI::DataTypes::IManifestObject& target) override;
        void GetPolicyName(AZStd::string& result) const override
        {
            result = "LoggingGroupBehavior";
        }

    private:
        static constexpr int s_loggingPreferredTabOrder{ 10 };
    };
} // namespace BopAudioScene
