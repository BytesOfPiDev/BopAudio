#pragma once

#include "Engine/Events/AudioTaskBase.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct StopSoundTask : public AudioTaskBase<StopSoundTask>
    {
        NamedResource m_resourceToStop{};
    };
} // namespace BopAudio
