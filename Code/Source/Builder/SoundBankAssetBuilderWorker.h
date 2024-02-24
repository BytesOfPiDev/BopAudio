#pragma once

#include <AssetBuilderSDK/AssetBuilderBusses.h>

namespace BopAudio
{
    class SoundBankAssetBuilderWorker : public AssetBuilderSDK::AssetBuilderCommandBus::Handler
    {
    public:
        AZ_RTTI(SoundBankAssetBuilderWorker, "{FECB2647-1224-4141-AEB6-BF4EE3064F18}");

        SoundBankAssetBuilderWorker() = default;

        void CreateJobs(
            AssetBuilderSDK::CreateJobsRequest const& request,
            AssetBuilderSDK::CreateJobsResponse& response) const;

        void ProcessJob(
            AssetBuilderSDK::ProcessJobRequest const& request,
            AssetBuilderSDK::ProcessJobResponse& response) const;

        void ShutDown() override
        {
            m_isShuttingDown = true;
        }

    private:
        AZStd::atomic_bool m_isShuttingDown{};
    };

} // namespace BopAudio
