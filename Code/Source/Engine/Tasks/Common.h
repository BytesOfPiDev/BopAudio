#pragma once

#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/std/containers/variant.h"

#include "Engine/Tasks/PlaySound.h"

namespace BopAudio
{
    static constexpr auto MaxTasks{ 3 };

    using AudioTask = AZStd::variant<PlaySoundTask>;
    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        AudioTask, "{7C4D381E-4240-4028-9298-36EE7DDE4E57}", "AudioTask");

    using TaskContainer = AZStd::vector<AudioTask>;
    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        TaskContainer, "{961FB915-9667-47CD-BA40-7C360935B7B6}", "AudioTaskContainer");
} // namespace BopAudio
