#pragma once

#include "AzCore/std/string/string.h"

namespace BopAudio
{
    // TODO: Let's hope I"m doing these right...

    template<typename T, AZStd::enable_if_t<true, bool>>
    auto ToString(T const& typeToConvert) -> AZStd::string
    {
        return typeToConvert.ToString();
    }

    template<typename T, AZStd::enable_if_t<true, bool>>
    auto ToCStr(T const& typeToConvert) -> char const*
    {
        return typeToConvert.c_str();
    }

    template<
        typename T,
        AZStd::enable_if_t<
            AZStd::is_same_v<decltype(AZStd::declval<T>().c_str()), char const*>,
            bool> = true>
    auto ToCStr(T const& typeToConvert) -> char const*
    {
        return typeToConvert.c_str();
    }

    template<
        typename T,
        AZStd::enable_if_t<
            AZStd::is_same_v<decltype(AZStd::declval<T>().ToString()), AZStd::string>,
            bool> = true>
    auto ToCStr(T const& typeToConvert) -> char const*
    {
        return typeToConvert.ToString().c_str();
    }

    template<
        typename T,
        AZStd::enable_if_t<
            AZStd::is_same_v<decltype(AZStd::declval<T>().GetCStr()), char const*>,
            bool> = true>
    auto ToCStr(T const& typeToConvert) -> char const*
    {
        return typeToConvert.GetCStr();
    }

} // namespace BopAudio
