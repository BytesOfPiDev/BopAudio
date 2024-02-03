/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root
 * of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AssetBuilderSDK/AssetBuilderBusses.h>

namespace BopAudio
{
    class AudioAssetBuilder : public AssetBuilderSDK::AssetBuilderCommandBus::Handler
    {
    public:
        AZ_RTTI(AudioAssetBuilder,
                "{2660F762-0814-431C-8FB0-CD2CF037E622}"); // NOLINT

        AudioAssetBuilder() = default;

        //! AssetBuilderCommandBus overrides ...
        void CreateJobs(
            AssetBuilderSDK::CreateJobsRequest const& request,
            AssetBuilderSDK::CreateJobsResponse& response) const;
        void ProcessJob(
            AssetBuilderSDK::ProcessJobRequest const& request,
            AssetBuilderSDK::ProcessJobResponse& response) const;
        void ShutDown() override
        {
        }
    };

} // namespace BopAudio
