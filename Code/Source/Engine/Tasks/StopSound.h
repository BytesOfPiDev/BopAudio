#pragma once

#include "Engine/Id.h"
#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    class AudioObject;
    struct StopSoundTask : public AudioTaskBase<StopSoundTask>
    {
        AZ_TYPE_INFO(StopSoundTask, "{0F608F88-2724-4C99-B95B-BE024C3DA0B1}");

        void operator()(AudioObject&){};
        ResourceRef m_resourceToStop{};
    };
} // namespace BopAudio
