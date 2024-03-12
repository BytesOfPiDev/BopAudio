#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/Utils/Utils.h"
#include "AzCore/std/string/string.h"

#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"

namespace BopAudio
{
    template<typename T>
    using AudioOutcome = AZ::Outcome<T, AZStd::string>;

    using NullOutcome = AudioOutcome<void>;

    /*
     * Returns just the name of a resource.
     *
     * @note The result has the same lifetime as the given argument.
     */
    static constexpr auto GetResourceName(ResourceRef const& ref) -> AZStd::string_view
    {
        AZ::IO::PathView path{ ref.GetCStr() };
        return path.Filename().Native();
    }

    static inline auto GetBankCachePath() -> AZ::IO::Path
    {
        return AZ::IO::Path{ BanksAlias };
    }

    static inline auto FindAssetId(AZ::IO::PathView assetPath, AZ::TypeId typeId)
        -> AZ::Data::AssetId
    {
        auto result = decltype(FindAssetId(assetPath, typeId)){};

        AZ::Data::AssetCatalogRequestBus::BroadcastResult(
            result,
            &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
            assetPath.Native().data(),
            typeId,
            true);

        return result;
    }

} // namespace BopAudio
