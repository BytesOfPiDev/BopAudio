#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetSerializer.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/std/containers/vector.h"

namespace BopAudio
{
    class AudioAsset : public AZ::Data::AssetData
    {
    public:
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioAsset);

        static constexpr auto FileExtension = "bafile";
        static constexpr auto AssetGroup = "Sound";
        static constexpr auto AssetSubId = 1u;

        static void Reflect(AZ::ReflectContext* context);

        AZStd::vector<AZ::u8> m_data;
    };

    using AudioAssetVector = AZStd::vector<AZ::Data::Asset<AudioAsset>>;
} // namespace BopAudio
