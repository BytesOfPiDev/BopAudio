#pragma once

#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/Script/ScriptContextAttributes.h"
namespace BopAudio
{
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
