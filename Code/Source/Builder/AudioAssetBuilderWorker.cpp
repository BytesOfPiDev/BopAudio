/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AssetBuilderSDK/AssetBuilderSDK.h>
#include <AssetBuilderSDK/SerializationDependencies.h>
#include <AzCore/Asset/AssetDataStream.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/IO/IOUtils.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzFramework/StringFunc/StringFunc.h>
#include <Builder/AudioAssetBuilderWorker.h>
#include <SteamAudio/AudioAsset.h>

namespace SteamAudio
{
    void AudioAssetBuilder::CreateJobs(
        const AssetBuilderSDK::CreateJobsRequest& request, AssetBuilderSDK::CreateJobsResponse& response) const
    {
    }

    void AudioAssetBuilder::ProcessJob(
        [[maybe_unused]] const AssetBuilderSDK::ProcessJobRequest& request, AssetBuilderSDK::ProcessJobResponse& response) const
    {
    }
} // namespace SteamAudio
