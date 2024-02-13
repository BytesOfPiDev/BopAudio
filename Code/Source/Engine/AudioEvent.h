#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>

#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/containers/fixed_vector.h"
#include "IAudioInterfacesCommonData.h"

#include "Engine/Id.h"
#include "Engine/Task.h"

namespace BopAudio
{
    class AudioEvent
    {
    public:
        static constexpr auto MaxTasks{ 3 };
        using TaskContainer = AZStd::fixed_vector<AudioTask, MaxTasks>;

        static auto Create(rapidjson::Document& doc, AZ::IO::Path const& eventPath)
            -> AZ::Outcome<AudioEvent, AZStd::string>;

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

        [[nodiscard]] auto GetResourceId() const -> NamedResource
        {
            return m_eventResourceId;
        }

        auto Execute() -> AZ::Outcome<void, char const*>;

    protected:
        AudioEvent() = default;

    private:
        NamedResource m_eventResourceId{};
        TaskContainer m_taskDatas{};
        Audio::EAudioEventState m_eventState{};
        InstanceId m_instanceId{};
    };

} // namespace BopAudio
