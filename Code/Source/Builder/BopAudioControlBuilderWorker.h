#pragma once

#include "AssetBuilderSDK/AssetBuilderBusses.h"
#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/Memory/Memory_fwd.h"
#include "AzCore/XML/rapidxml.h"
#include "AzCore/std/parallel/atomic.h"

namespace BopAudio
{
    class BopAudioControlBuilderWorker : public AssetBuilderSDK::AssetBuilderCommandBus::Handler
    {
    public:
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(BopAudioControlBuilderWorker);
        AZ_CLASS_ALLOCATOR_DECL;

        //! AssetBuilderCommandBus overrides ...
        void CreateJobs(AssetBuilderSDK::CreateJobsRequest const& request, AssetBuilderSDK::CreateJobsResponse& response);
        void ProcessJob(AssetBuilderSDK::ProcessJobRequest const& request, AssetBuilderSDK::ProcessJobResponse& response);
        void ShutDown() override;

        auto GatherProductDependencies(
            AZStd::string const& fullPath, AZStd::string const& relativePath, AssetBuilderSDK::ProductPathDependencySet& dependencies)
            -> AZ::Outcome<AZStd::string, AZStd::string>;

        auto ParseProductDependencies(
            AssetBuilderSDK::ProcessJobRequest const& request,
            AZStd::vector<AssetBuilderSDK::ProductDependency>& productDependencies,
            AssetBuilderSDK::ProductPathDependencySet& pathDependencies) -> bool;

    private:
        void ParseProductDependenciesFromXmlFile(
            const AZ::rapidxml::xml_node<char>* node,
            AZStd::string const& fullPath,
            AZStd::string const& sourceFile,
            AZStd::string const& platformIdentifier,
            AZStd::vector<AssetBuilderSDK::ProductDependency>& productDependencies,
            AssetBuilderSDK::ProductPathDependencySet& pathDependencies);

    private:
        AZStd::string m_globalScopeControlsPath;
        AZStd::atomic_bool m_isShuttingDown{};
    };
} // namespace BopAudio
