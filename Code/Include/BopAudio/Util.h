#pragma once

#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/string/string.h"
#include "Engine/Id.h"
#include <AzCore/IO/Path/Path.h>

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
} // namespace BopAudio
