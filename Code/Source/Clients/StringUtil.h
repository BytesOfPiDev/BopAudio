#pragma once

#include "AzCore/std/string/string.h"
#include <AzCore/std/typetraits/is_convertible.h>

namespace BopAudio
{
    template<typename T>
    auto ToString(T const& typeToConvert) -> AZStd::string
    {
        static_assert(
            AZStd::is_convertible_v<std::result_of_t<decltype (&T::ToString)()>, AZStd::string>,
            "The type must have a ToString() function that returns an AZStd::string.");
        return typeToConvert.ToString();
    }

    template<typename T>
    auto ToCStr(T const& typeToConvert) -> char const*
    {
        static_assert(
            AZStd::is_convertible_v<decltype(typeToConvert.ToString().c_str()), char const*>,
            "The type must have a ToString() function that returns 'char const*'");
        return typeToConvert.ToString().c_str();
    }

} // namespace BopAudio
