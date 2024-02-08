#include "Engine/AudioEvent.h"

namespace BopAudio
{
    AudioEvent::AudioEvent(TaskContainer tasks)
        : m_taskDatas{ AZStd::move(tasks) } {};
} // namespace BopAudio
