#pragma once

#include "AzCore/std/containers/variant.h"

namespace BopAudio
{

    struct PlaySoundArgs;
    struct StopSoundArgs;
    struct SetVolumeArgs;
    struct TranslateArgs;
    struct MuteArgs;

    using TaskArgs = AZStd::variant<PlaySoundArgs, StopSoundArgs>;

    struct AudioEventTask
    {
        constexpr void operator()(PlaySoundArgs const& args);
        constexpr void operator()(StopSoundArgs const& args);
        constexpr void operator()(SetVolumeArgs const& args);
        constexpr void operator()(TranslateArgs const& args);
        constexpr void operator()(MuteArgs const& args);
    };

} // namespace BopAudio
