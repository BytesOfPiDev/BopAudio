#pragma once

#include "AzCore/std/containers/variant.h"

#include "Engine/Events/PlaySound.h"
#include "Engine/Events/StopSound.h"

namespace BopAudio
{

    using AudioTask = AZStd::variant<PlaySoundTask, StopSoundTask>;

    struct AudioTaskHandler
    {
        void operator()(PlaySoundTask const& args) const;
        void operator()(StopSoundTask const& args) const;
    };

    struct AudioTaskIsFinished
    {
        void operator()(PlaySoundTask const& args) const;
        void operator()(StopSoundTask const& args) const;
    };

} // namespace BopAudio
