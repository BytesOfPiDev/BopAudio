#include "Builder/AudioEventAssetBuilderWorker.h"

#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/StringFunc/StringFunc.h"

namespace BopAudio
{
    namespace Internal
    {
        static constexpr auto BuildAudioEventJobKey = "Build AudioEvent";
    }

    void AudioEventAssetBuilderWorker::CreateJobs(
        AssetBuilderSDK::CreateJobsRequest const& request,
        AssetBuilderSDK::CreateJobsResponse& response) const
    {
        if (m_isShuttingDown)
        {
            response.m_result = AssetBuilderSDK::CreateJobsResultCode::ShuttingDown;
            return;
        }

        AZStd::ranges::for_each(
            request.m_enabledPlatforms,
            [&response](auto const& info)
            {
                if (info.m_identifier == "server")
                {
                    return;
                }

                AssetBuilderSDK::JobDescriptor descriptor;
                descriptor.m_jobKey = Internal::BuildAudioEventJobKey;
                descriptor.m_critical = true;
                descriptor.SetPlatformIdentifier(info.m_identifier.c_str());
                descriptor.m_priority = 0;
                response.m_createJobOutputs.push_back(descriptor);
            });

        response.m_result = AssetBuilderSDK::CreateJobsResultCode::Success;
    }

    void AudioEventAssetBuilderWorker::ProcessJob(
        AssetBuilderSDK::ProcessJobRequest const& request,
        AssetBuilderSDK::ProcessJobResponse& response) const
    {
        if (m_isShuttingDown)
        {
            AZ_Trace(
                AssetBuilderSDK::WarningWindow,
                "Cancelling job %s due to shutdown request.",
                request.m_fullPath.c_str());

            return;
        }

        if (!AZ::StringFunc::Equal(
                request.m_jobDescription.m_jobKey, Internal::BuildAudioEventJobKey))
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Trace(AssetBuilderSDK::ErrorWindow, "Job failed. Unsupported job key: '%s'") return;

            return;
        }

        AssetBuilderSDK::JobCancelListener jobCancelListener(request.m_jobId);

        if (jobCancelListener.IsCancelled())
        {
            AZ_Trace(
                AssetBuilderSDK::WarningWindow,
                "Cancel was requested for job %s.\n",
                request.m_fullPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Cancelled;
            return;
        }
    }
} // namespace BopAudio
