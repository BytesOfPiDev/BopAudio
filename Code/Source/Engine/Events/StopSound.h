#pragma once

#include "Engine/Events/AudioEventBase.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct StopSoundData : public AudioEventBase<StopSoundData>
    {
        NamedResource m_resourceId{};
    };
} // namespace BopAudio
