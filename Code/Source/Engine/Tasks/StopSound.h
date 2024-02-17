#pragma once

#include "Engine/AudioObject.h"
#include "Engine/Id.h"
#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    struct StopSoundTask : public AudioTaskBase<StopSoundTask>
    {
        void operator()(AudioObject&){};
        ResourceRef m_resourceToStop{};
    };
} // namespace BopAudio
