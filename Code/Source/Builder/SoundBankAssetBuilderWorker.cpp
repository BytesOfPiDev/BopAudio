#include "Builder/SoundBankAssetBuilderWorker.h"

#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/Asset/AssetDataStream.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "rapidjson/document.h"

#include "Clients/SoundBankAsset.h"
#include "Engine/Id.h"
#include "Engine/SoundBankUtil.h"

namespace BopAudio
{
    namespace Internal
    {
        static constexpr auto BuildSoundBankJobKey = "Build SoundBank";
    }

    auto LoadSounds(rapidjson::Document const& doc) -> AZStd::vector<SoundSource>
    {
        auto const soundNames = GetSoundNamesFromSoundBankFile(doc);
        AZStd::vector<SoundSource> sounds{};

        AZStd::ranges::for_each(
            soundNames,
            [&sounds](auto const& soundName) -> bool
            {
                SoundSource soundSource{ AZ::IO::Path{ soundName.GetCStr() } };
                soundSource.Load();

                if (!soundSource.IsReady())
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to find sound asset '%s'.",
                        soundName.GetCStr());
                    return false;
                }

                sounds.emplace_back(soundSource);

                return true;
            });

        return sounds;
    }

    void SoundBankAssetBuilderWorker::CreateJobs(
        AssetBuilderSDK::CreateJobsRequest const& request,
        AssetBuilderSDK::CreateJobsResponse& response) const
    {
        AZ_Error("SoundBankAssetBuilderWorker", false, "Creating jobs ...");
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
                descriptor.m_jobKey = Internal::BuildSoundBankJobKey;
                descriptor.m_critical = true;
                descriptor.SetPlatformIdentifier(info.m_identifier.c_str());
                descriptor.m_priority = 0;
                response.m_createJobOutputs.push_back(descriptor);
            });

        response.m_result = AssetBuilderSDK::CreateJobsResultCode::Success;
    }

    void SoundBankAssetBuilderWorker::ProcessJob(
        [[maybe_unused]] AssetBuilderSDK::ProcessJobRequest const& request,
        AssetBuilderSDK::ProcessJobResponse& response) const
    {
        if (m_isShuttingDown)
        {
            AZ_Warning(
                AssetBuilderSDK::WarningWindow,
                false,
                "Cancelling job %s due to shutdown request.",
                request.m_fullPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        // Verify job key is correct
        if (!AZ::StringFunc::Equal(
                request.m_jobDescription.m_jobKey, Internal::BuildSoundBankJobKey))
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Job failed. Unsupported job key: '%s'");

            return;
        }

        AssetBuilderSDK::JobCancelListener jobCancelListener(request.m_jobId);

        AZStd::vector<char> const soundBankBuffer = [&request]() -> decltype(soundBankBuffer)
        {
            auto loadBufferOutcome{ LoadSoundBankToBuffer(
                AZ::IO::Path{ request.m_fullPath.c_str() }) };

            if (!loadBufferOutcome.IsSuccess())
            {
                AZ_Error(
                    AssetBuilderSDK::ErrorWindow,
                    false,
                    "Failed to load sound bank buffer due to error: [%s]",
                    loadBufferOutcome.GetError().c_str());

                return {};
            }

            AZ_Info(
                AssetBuilderSDK::InfoWindow,
                "Successfully loaded sound bank buffer: [%s]",
                request.m_sourceFile.c_str());

            return loadBufferOutcome.TakeValue();
        }();

        if (soundBankBuffer.empty())
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow, false, "Failed to load soundbank file to buffer.");
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

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

        rapidjson::Document doc{};
        doc.Parse(soundBankBuffer.data(), soundBankBuffer.size());

        AZ::IO::Path const productPath = [&request]() -> decltype(productPath)
        {
            AZ::IO::Path path{ request.m_sourceFile };
            path.ReplaceExtension(SoundBankAsset::ProductExtension);
            return path;
        }();

        AZ::IO::Path const tempPath = [&request, &productPath]() -> decltype(tempPath)
        {
            AZ::IO::Path path{ request.m_tempDirPath };
            path /= productPath.Filename();

            return path;
        }();

        SoundBankAsset soundBank{};
        // Sound banks are referenced by their path relative to the DefaultBanksPath.
        soundBank.m_id = BankRef{ productPath.Native() };
        soundBank.m_soundSources = LoadSounds(doc);

        bool const successfullySaved = AZ::Utils::SaveObjectToFile<SoundBankAsset>(
            tempPath.c_str(), AZ::DataStream::ST_JSON, &soundBank);

        if (!successfullySaved)
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Job failed. Could not save to temporary folder.");

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        AssetBuilderSDK::JobProduct jobProduct{ tempPath.c_str() };
        jobProduct.m_productAssetType = AZ::AzTypeInfo<SoundBankAsset>::Uuid();
        jobProduct.m_productSubID = SoundBankAsset::AssetSubId;
        jobProduct.m_dependenciesHandled = true;

        response.m_outputProducts.push_back(jobProduct);
        response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Success;
    }

} // namespace BopAudio
