#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/Script/ScriptContextAttributes.h"
#include "AzCore/std/string/string.h"
namespace BopAudio
{
    template<typename T>
    using AudioOutcome = AZ::Outcome<T, AZStd::string>;

    using NullOutcome = AudioOutcome<void>;

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

    static inline auto GetAssetPath(AZ::Data::AssetId const& id) -> AZ::IO::Path
    {
        return [&id]() -> decltype(GetAssetPath(id))
        {
            auto result{ decltype(GetAssetPath(id)){} };
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                result, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetPathById, id);

            return result;
        }();
    }

    template<typename T>
    static constexpr auto EnableTypeForScriptEventUsage(
        AZ::BehaviorContext::ClassBuilder<T>& classBuilder) -> AZ::BehaviorContext::ClassBuilder<T>*
    {
        static_assert(AZStd::is_pod_v<T>, "For now, only POD types are supported.");
        return classBuilder
            ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
            ->Attribute(AZ::Script::Attributes::ConstructibleFromNil, true)
            ->Attribute(AZ::Script::Attributes::EnableAsScriptEventParamType, true)
            ->Attribute(AZ::Script::Attributes::EnableAsScriptEventReturnType, true);
    }

} // namespace BopAudio
