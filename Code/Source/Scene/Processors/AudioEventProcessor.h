#pragma once

#include "SceneAPI/SceneCore/Components/LoadingComponent.h"
#include "SceneAPI/SceneCore/Events/AssetImportRequest.h"

namespace BopAudioScene
{
    class LoadingTrackingProcessor
        : public AZ::SceneAPI::SceneCore::LoadingComponent
        , public AZ::SceneAPI::Events::AssetImportRequestBus::Handler
    {
    public:
        AZ_COMPONENT(
            LoadingTrackingProcessor,
            "{B81601D7-BD0D-4D0B-92BF-88763AAE6670}",
            AZ::SceneAPI::SceneCore::LoadingComponent);
        AZ_DISABLE_COPY_MOVE(LoadingTrackingProcessor);

        LoadingTrackingProcessor();
        ~LoadingTrackingProcessor() override = default;

        void Activate() override;
        void Deactivate() override;
        static void Reflect(AZ::ReflectContext* context);

        auto PrepareForAssetLoading(
            AZ::SceneAPI::Containers::Scene& scene, RequestingApplication requester)
            -> AZ::SceneAPI::Events::ProcessingResult override;

        auto LoadAsset(
            AZ::SceneAPI::Containers::Scene& scene,
            AZStd::string const& path,
            const AZ::Uuid& guid,
            RequestingApplication requester) -> AZ::SceneAPI::Events::LoadingResult override;

        void FinalizeAssetLoading(
            AZ::SceneAPI::Containers::Scene& scene, RequestingApplication requester) override;

        auto UpdateManifest(
            AZ::SceneAPI::Containers::Scene& scene,
            ManifestAction action,
            RequestingApplication requester) -> AZ::SceneAPI::Events::ProcessingResult override;

        void GetPolicyName(AZStd::string& result) const override;

        [[nodiscard]] auto GetPriority() const -> uint8_t override;

        auto ContextCallback(AZ::SceneAPI::Events::ICallContext& context)
            -> AZ::SceneAPI::Events::ProcessingResult;
    };
} // namespace BopAudioScene
