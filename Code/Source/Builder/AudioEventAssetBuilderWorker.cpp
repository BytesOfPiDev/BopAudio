#include "Builder/AudioEventAssetBuilderWorker.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/Debug/Trace.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/StringFunc/StringFunc.h"

#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    namespace Internal
    {
        static constexpr auto BuildAudioEventJobKey = "Build Audio Event";
        static constexpr auto CopyAudioEventJobKey = "Copy Audio Event";
        static constexpr auto InvalidAudioEventJobKey = "Invalid Audio Event";

    } // namespace Internal

    AudioEventAssetBuilderWorker::AudioEventAssetBuilderWorker() = default;

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
            [&response, &request](auto const& info)
            {
                if (info.m_identifier == "server")
                {
                    return;
                }

                AZ::IO::PathView sourcePath{ request.m_sourceFile };
                AZ_Info(AssetBuilderSDK::InfoWindow, "SourcePath: %s", sourcePath.Native().data());

                AssetBuilderSDK::JobDescriptor descriptor;
                if (AZ::StringFunc::Path::IsExtension(
                        request.m_sourceFile.c_str(), AudioEventAsset::SourceExtensionPattern))
                {
                    descriptor.m_jobKey = Internal::BuildAudioEventJobKey;
                }
                else if (AZ::StringFunc::Path::IsExtension(
                             request.m_sourceFile.c_str(),
                             AudioEventAsset::ProductExtensionPattern))
                {
                    descriptor.m_jobKey = Internal::CopyAudioEventJobKey;
                }
                else
                {
                    descriptor.m_jobKey = Internal::InvalidAudioEventJobKey;
                    AZ_Error(
                        AssetBuilderSDK::ErrorWindow,
                        false,
                        "Asset is neither a source nor product asset.");
                }
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
        static constexpr auto isJobKey = [](auto const& request, auto const& key) -> bool
        {
            return AZ::StringFunc::Equal(request.m_jobDescription.m_jobKey, key);
        };

        if (isJobKey(request, Internal::BuildAudioEventJobKey))
        {
            AZ_Info(
                AssetBuilderSDK::InfoWindow,
                "Processing '%s' key.",
                Internal::BuildAudioEventJobKey);
        }
        else if (isJobKey(request, Internal::CopyAudioEventJobKey))
        {
            AZ_Info(
                AssetBuilderSDK::InfoWindow,
                "Processing '%s' key.",
                Internal::CopyAudioEventJobKey);
            Copy(request, response);
        }
        else
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Job failed. Unsupported job key: '%s'") return;
        }
    }

    void AudioEventAssetBuilderWorker::Copy(
        AssetBuilderSDK::ProcessJobRequest const& request,
        AssetBuilderSDK::ProcessJobResponse& response) const
    {
        if (m_isShuttingDown)
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Cancelling job %s due to shutdown request.",
                request.m_fullPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Cancelled;
            return;
        }
        AssetBuilderSDK::JobCancelListener jobCancelListener(request.m_jobId);

        if (jobCancelListener.IsCancelled())
        {
            AZ_Warning(
                AssetBuilderSDK::WarningWindow,
                false,
                "Cancel was requested for job %s.\n",
                request.m_fullPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Cancelled;
            return;
        }

        if (!AZ::StringFunc::Path::IsExtension(
                request.m_sourceFile.c_str(), AudioEventAsset::ProductExtensionPattern))
        {
            AZ_Error(AssetBuilderSDK::ErrorWindow, false, "The file has the wrong extension!");
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        AZStd::string const sourceFilename = [&request]() -> decltype(sourceFilename)
        {
            AZStd::string filename{};
            AZ::StringFunc::Path::GetFileName(request.m_sourceFile.c_str(), filename);

            return filename;
        }();

        AZStd::string const absProductPath = [&request]() -> decltype(absProductPath)
        {
            AZStd::string path{};
            AZ::StringFunc::Path::Join(
                request.m_tempDirPath.c_str(), request.m_sourceFile.c_str(), path);

            AZ_Assert(
                AZ::StringFunc::Path::IsExtension(
                    path.c_str(), AudioEventAsset::ProductExtensionPattern),
                "The file has the wrong extension!");

            return path;
        }();

        auto asset = AZStd::unique_ptr<AudioEventAsset>{
            AZ::Utils::LoadObjectFromFile<AudioEventAsset>(request.m_fullPath)
        };
        if (asset == nullptr)
        {
            AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Failed to load asset.");

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        auto const successfulCopy{ AZ::IO::FileIOBase::GetInstance()->Copy(
            request.m_fullPath.c_str(), absProductPath.c_str()) };

        if (!successfulCopy)
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Failed to copy pre-existing product asset from [%s] to [%s].",
                request.m_fullPath.c_str(),
                absProductPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        AssetBuilderSDK::JobProduct jobProduct{ absProductPath.c_str() };
        jobProduct.m_productAssetType = AZ::AzTypeInfo<AudioEventAsset>::Uuid();
        jobProduct.m_productSubID = AudioEventAsset::AssetSubId;
        jobProduct.m_dependenciesHandled = false;

        response.m_outputProducts.push_back(jobProduct);

        response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Success;
    }
} // namespace BopAudio
