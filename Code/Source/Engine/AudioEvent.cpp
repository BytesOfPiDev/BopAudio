#include "Engine/AudioEvent.h"

#include <AzCore/JSON/pointer.h>

#include "AzCore/Console/ILogger.h"
#include "AzCore/std/string/conversions.h"
#include "AzCore/std/string/string.h"

#include "Clients/StringUtil.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/Events/PlaySound.h"
#include "Engine/Events/StopSound.h"
#include "Engine/Task.h"

namespace BopAudio
{
    auto AudioEvent::Create(rapidjson::Document& doc, AZ::IO::Path const& eventPath)
        -> AZ::Outcome<AudioEvent, AZStd::string>
    {
        AZStd::vector<AZ::IO::Path> taskPaths{};
        rapidjson::Pointer const eventJsonPtr(ToCStr(eventPath));

        AudioEvent event{};
        auto eventObject = rapidjson::GetValueByPointer(doc, eventJsonPtr);

        if (!eventObject || !eventObject->IsObject())
        {
            return AZ::Failure(
                AZStd::string::format("Failed to find event with key: [%s]", ToCStr(eventPath)));
        }

        // We expect the event object to have a Tasks member
        auto tasksMember = eventObject->FindMember("Tasks");
        if (tasksMember == eventObject->MemberEnd())
        {
            return AZ::Failure(
                AZStd::string::format("Failed to find expected key: %s/Tasks.", ToCStr(eventPath)));
        }

        // That Tasks member must be an array.
        if (!tasksMember->value.IsArray())
        {
            return AZ::Failure(AZStd::string::format(
                "Expected array at '%s/Tasks', but got something else.",
                ToCStr(eventPath.String())));
        }

        auto const tasksArray{ tasksMember->value.GetArray() };
        auto const taskBasePath{ eventPath / "Tasks" };

        for (auto index{ 0 }; index < tasksArray.Size(); ++index)
        {
            auto const& key{ taskPaths.emplace_back(taskBasePath / AZStd::to_string(index)) };
            AZLOG(ASI_miniaudio, "Found event TaskGroup: '%s'.", ToCStr(key));
        }

        AZStd::vector<AudioTask> taskDatas{};

        for (AZ::IO::Path const& taskPath : taskPaths)
        {
            rapidjson::Pointer const taskJsonPtr{ ToCStr(taskPath) };
            auto* const taskValue{ rapidjson::GetValueByPointer(doc, taskJsonPtr) };

            if (!taskValue || !taskValue->IsObject())
            {
                continue;
            }

            auto const taskObject{ taskValue->GetObject() };

            // TODO: Need to find a better way to build each task. Visitor? Busses with Id for
            // each Task?

            if (auto* playResourceValue{
                    rapidjson::GetValueByPointer(taskObject, JsonKeys::PlayResourceKey) })
            {
                AudioTask& playTask{ taskDatas.emplace_back(PlaySoundTask{}) };
                AZStd::get<PlaySoundTask>(playTask).m_resourceToPlay =
                    NamedResource{ playResourceValue->GetString() };

                AZLOG_INFO("Built Task: %s.", ToCStr(taskPath));
            }

            if (auto* stopResourceValue{
                    rapidjson::GetValueByPointer(taskObject, JsonKeys::StopEventResourceKey) })
            {
                AudioTask& stopTask{ taskDatas.emplace_back(StopSoundTask{}) };
                AZStd::get<StopSoundTask>(stopTask).m_resourceToStop =
                    NamedResource{ stopResourceValue->GetString() };
            }
        };

        return AZ::Success(AudioEvent());
    }

    auto AudioEvent::Execute() -> AZ::Outcome<void, char const*>
    {
        static constexpr AudioTaskHandler handler{};
        for (AudioTask const& task : m_taskDatas)
        {
            AZStd::visit(handler, task);
        }

        for (AudioTask const& task : m_taskDatas)
        {
            AZStd::visit(AudioTaskIsFinished{}, task);
        }

        return AZ::Success();
    }

} // namespace BopAudio
