#include "Scene/Processors/AudioEventProcessor.h"

#include "AzCore/Serialization/SerializeContext.h"
#include "SceneAPI/SceneCore/Utilities/Reporting.h"

namespace BopAudioScene
{
    void LoadingTrackingProcessor::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext
                ->Class<LoadingTrackingProcessor, AZ::SceneAPI::SceneCore::LoadingComponent>()
                ->Version(1);
        }
    }

    LoadingTrackingProcessor::LoadingTrackingProcessor()
    {
        // For details about the CallProcessorBus and CallProcessorBinder, see
        // ExportTrackingProcessor.cpp.
        BindToCall(
            &LoadingTrackingProcessor::ContextCallback,
            AZ::SceneAPI::Events::CallProcessorBinder::TypeMatch::Derived);
    }

    void LoadingTrackingProcessor::Activate()
    {
        AZ::SceneAPI::Events::AssetImportRequestBus::Handler::BusConnect();

        // Forward the call to the LoadingComponent so that the call bindings get activated.
        AZ::SceneAPI::SceneCore::LoadingComponent::Activate();
    }

    void LoadingTrackingProcessor::Deactivate()
    {
        AZ::SceneAPI::SceneCore::LoadingComponent::Deactivate();

        // Forward the call to the LoadingComponent so that the call bindings get deactivated.
        AZ::SceneAPI::Events::CallProcessorBus::Handler::BusDisconnect();

        AZ::SceneAPI::Events::AssetImportRequestBus::Handler::BusDisconnect();
    }

    auto LoadingTrackingProcessor::PrepareForAssetLoading(
        [[maybe_unused]] AZ::SceneAPI::Containers::Scene& scene,
        [[maybe_unused]] RequestingApplication requester) -> AZ::SceneAPI::Events::ProcessingResult
    {
        AZ_TracePrintf(AZ::SceneAPI::Utilities::LogWindow, "Preparing to load a scene.");
        // This function doesn't contribute anything to the loading, so let the SceneAPI know that
        // it can ignore its contributions.
        return AZ::SceneAPI::Events::ProcessingResult::Ignored;
    }

    auto LoadingTrackingProcessor::LoadAsset(
        [[maybe_unused]] AZ::SceneAPI::Containers::Scene& scene,
        AZStd::string const& path,
        [[maybe_unused]] const AZ::Uuid& guid,
        [[maybe_unused]] RequestingApplication requester) -> AZ::SceneAPI::Events::LoadingResult
    {
        AZ_TracePrintf(
            AZ::SceneAPI::Utilities::LogWindow, "Loading scene from '%s'.", path.c_str());
        return AZ::SceneAPI::Events::LoadingResult::Ignored;
    }

    void LoadingTrackingProcessor::FinalizeAssetLoading(
        [[maybe_unused]] AZ::SceneAPI::Containers::Scene& scene,
        [[maybe_unused]] RequestingApplication requester)
    {
        AZ_TracePrintf(AZ::SceneAPI::Utilities::LogWindow, "Finished loading scene.");
    }

    auto LoadingTrackingProcessor::UpdateManifest(
        [[maybe_unused]] AZ::SceneAPI::Containers::Scene& scene,
        ManifestAction action,
        [[maybe_unused]] RequestingApplication requester) -> AZ::SceneAPI::Events::ProcessingResult
    {
        switch (action)
        {
        case ManifestAction::ConstructDefault:
            AZ_TracePrintf(AZ::SceneAPI::Utilities::LogWindow, "Constructing a new manifest.");
            break;
        case ManifestAction::Update:
            AZ_TracePrintf(AZ::SceneAPI::Utilities::LogWindow, "Updating the manifest.");
            break;
        default:
            AZ_TracePrintf(
                AZ::SceneAPI::Utilities::WarningWindow, "Unknown manifest update action.");
            break;
        }
        return AZ::SceneAPI::Events::ProcessingResult::Ignored;
    }

    void LoadingTrackingProcessor::GetPolicyName(AZStd::string& result) const
    {
        result = "LoadingTrackingProcessor";
    }

    [[nodiscard]] auto LoadingTrackingProcessor::GetPriority() const -> uint8_t
    {
        return EarliestProcessing;
    }

    auto LoadingTrackingProcessor::ContextCallback(AZ::SceneAPI::Events::ICallContext& context)
        -> AZ::SceneAPI::Events::ProcessingResult
    {
        AZ_TracePrintf(
            AZ::SceneAPI::Utilities::LogWindow, "LoadEvent: %s", context.RTTI_GetTypeName());
        return AZ::SceneAPI::Events::ProcessingResult::Ignored;
    }
} // namespace BopAudioScene
