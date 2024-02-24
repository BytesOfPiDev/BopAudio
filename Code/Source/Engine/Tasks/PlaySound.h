#pragma once

#include "Engine/Id.h"
#include "Engine/Tasks/AudioTaskBase.h"
#include "Engine/Tasks/TaskBus.h"

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

    class PlayTaskFactory : public TaskFactoryBus::Handler
    {
        [[nodiscard]] auto Create(AZStd::span<char const>) const -> AZStd::any override
        {
            return {};
        };
    };
} // namespace BopAudio
