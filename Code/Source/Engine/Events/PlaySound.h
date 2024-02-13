#pragma once

#include "Engine/Events/AudioTaskBase.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct PlaySoundTask : public AudioTaskBase<PlaySoundTask>
    {
        NamedResource m_resourceToPlay;
    };
} // namespace BopAudio
