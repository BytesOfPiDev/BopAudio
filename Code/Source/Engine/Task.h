#pragma once

#include "AzCore/std/containers/variant.h"

namespace BopAudio
{
    struct MuteArgs;
    struct PlayGroupArgs;
    struct PlaySoundData;
    struct SetVolumeArgs;
    struct StopSoundData;
    struct TranslateArgs;

    using TaskData = AZStd::variant<PlaySoundData, StopSoundData>;

    struct AudioEventHandler
    {
        constexpr void operator()(PlaySoundData const& args);
        constexpr void operator()(StopSoundData const& args);
        constexpr void operator()(SetVolumeArgs const& args);
        constexpr void operator()(TranslateArgs const& args);
        constexpr void operator()(MuteArgs const& args);
    };

} // namespace BopAudio
