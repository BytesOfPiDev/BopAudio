#pragma once

#include "Engine/Events/AudioEventBase.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct PlaySoundData : public AudioEventBase<PlaySoundData>
    {
        NamedResource m_resourceId{};
    };
} // namespace BopAudio
