#pragma once

#include "AzCore/std/containers/variant.h"

#include "Engine/Tasks/PlaySound.h"

namespace BopAudio
{
    using AudioTask = AZStd::variant<PlaySoundTask>;
    using TaskContainer = AZStd::vector<AudioTask>;
} // namespace BopAudio
