#pragma once

#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    template<typename AudioEventDerived>
    struct AudioEventBase
    {
        Audio::EAudioEventState m_eventState{};
    };
} // namespace BopAudio
