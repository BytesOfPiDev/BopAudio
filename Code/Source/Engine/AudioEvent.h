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

        AudioEvent() = default;

        [[nodiscard]] constexpr auto GetInstanceId() const -> InstanceId
        {
            return m_instanceId;
        }

        [[nodiscard]] constexpr auto GetState() const -> Audio::EAudioEventState
        {
            return m_eventState;
        }

    private:
        AZStd::fixed_vector<TaskData, MaxTasks> m_taskDatas{};
        Audio::EAudioEventState m_eventState{};
        InstanceId m_instanceId{};

    public:
        [[nodiscard]] constexpr auto GetTaskDatas() const -> decltype(m_taskDatas) const&
        {
            return m_taskDatas;
        }
    };
} // namespace BopAudio
