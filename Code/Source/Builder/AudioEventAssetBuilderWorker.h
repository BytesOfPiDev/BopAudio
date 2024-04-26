#pragma once

#include "AssetBuilderSDK/AssetBuilderBusses.h"

namespace BopAudio
{
    class AudioEventAssetBuilderWorker : public AssetBuilderSDK::AssetBuilderCommandBus::Handler
    {
    public:
        AZ_RTTI(AudioEventAssetBuilderWorker, "{DEBDCF13-0949-49D0-979D-B05288A4FB25}");

        AudioEventAssetBuilderWorker();

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

    protected:
        void CreateFactories();

        void Copy(
            AssetBuilderSDK::ProcessJobRequest const& request,
            AssetBuilderSDK::ProcessJobResponse& response) const;

    private:
        AZStd::atomic_bool m_isShuttingDown{};
    };
} // namespace BopAudio
