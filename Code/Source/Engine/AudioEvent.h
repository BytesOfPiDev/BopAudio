#pragma once

#include "AzCore/std/containers/fixed_vector.h"
#include "Engine/Task.h"

#include <Engine/Events/PlaySound.h>
#include <Engine/Events/StopSound.h>

namespace BopAudio
{

    class AudioEvent
    {
    public:
        static constexpr auto MaxTasks{ 3 };
        using TaskContainer = AZStd::fixed_vector<TaskData, MaxTasks>;

        AudioEvent() = default;
        AudioEvent(TaskContainer tasks);

        [[nodiscard]] constexpr auto GetInstanceId() const -> InstanceId
        {
            return m_instanceId;
        }

        [[nodiscard]] constexpr auto GetState() const -> Audio::EAudioEventState
        {
            return m_eventState;
        }

        [[nodiscard]] constexpr auto GetTaskDatas() const -> TaskContainer const&
        {
            return m_taskDatas;
        }

    private:
        TaskContainer m_taskDatas{};
        Audio::EAudioEventState m_eventState{};
        InstanceId m_instanceId{};

    public:
    };
} // namespace BopAudio
