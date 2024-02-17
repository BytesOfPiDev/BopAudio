#pragma once

#include "Engine/Id.h"
#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    class AudioObject;

    struct PlaySoundTask : public AudioTaskBase<PlaySoundTask>
    {
        void operator()(AudioObject&)
        {
            AZ_Info("PlaySoundTask", "Play: [%]", m_resourceToPlay.GetCStr());
        }
        ResourceRef m_resourceToPlay;
    };
} // namespace BopAudio
