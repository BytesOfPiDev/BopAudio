#include "Engine/AudioEvent.h"

#include <AzCore/JSON/pointer.h>

#include "AzCore/Console/ILogger.h"
#include "AzCore/std/string/conversions.h"
#include "AzCore/std/string/string.h"

#include "Engine/AudioObject.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/Id.h"
#include "Engine/Tasks/PlaySound.h"
#include "Engine/Tasks/StopSound.h"

namespace BopAudio
{
    namespace Internal
    {
        AZ_HAS_MEMBER(RunMember, operator(), void, (AudioObjectId));

        static constexpr auto MaxTasks{ 3 };
        using Task = AZStd::variant<PlaySoundTask, StopSoundTask>;
        using TaskContainer = AZStd::fixed_vector<Task, MaxTasks>;

    } // namespace Internal

    AZ_TYPE_INFO_WITH_NAME_IMPL(AudioEvent, "AudioEvent", "{89BF5BB5-63C8-43A1-8D0B-F80ED2A30C15}");
    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        Internal::TaskContainer,
        "{961FB915-9667-47CD-BA40-7C360935B7B6}",
        "AudioEventInternalTaskContainer");

    auto AudioEvent::CreateFromSource(rapidjson::Document& doc, AZ::IO::Path const& path)
        -> AudioOutcome<AudioEvent>
    {
        return CreateFromSource(doc, ResourceRef{ path.Native() });
    }

    auto AudioEvent::CreateFromSource(rapidjson::Document& doc, ResourceRef const& resourceRef)
        -> AudioOutcome<AudioEvent>
    {
        AZStd::vector<AZ::IO::Path> taskPaths{};
        rapidjson::Pointer const eventJsonPtr(resourceRef.GetCStr());

        auto eventObject = rapidjson::GetValueByPointer(doc, eventJsonPtr);

        if (!eventObject || !eventObject->IsObject())
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to find event with key: [%s]", resourceRef.GetCStr()));
        }

        // We expect the event object to have a Tasks member
        auto tasksMember = eventObject->FindMember(AudioStrings::TaskTag);
        if (tasksMember == eventObject->MemberEnd())
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to find expected key: [%s/%s].",
                resourceRef.GetCStr(),
                AudioStrings::TaskTag));
        }

        // That Tasks member must be an array.
        if (!tasksMember->value.IsArray())
        {
            return AZ::Failure(AZStd::string::format(
                "Expected array at '%s/%s', but got something else.",
                resourceRef.GetCStr(),
                AudioStrings::TaskTag));
        }

        auto const tasksArray{ tasksMember->value.GetArray() };
        auto const taskBasePath{ AZ::IO::Path{ resourceRef.GetCStr() } / AudioStrings::TaskTag };

        for (auto index{ 0 }; index < tasksArray.Size(); ++index)
        {
            auto const& key{ taskPaths.emplace_back(taskBasePath / AZStd::to_string(index)) };
            AZLOG(ASI_miniaudio, "Found event TaskGroup: '%s'.", key.c_str());
        }

        Internal::TaskContainer taskDatas{};

        for (AZ::IO::Path const& taskPath : taskPaths)
        {
            rapidjson::Pointer const taskJsonPtr{ taskPath.c_str() };
            auto* const taskValue{ rapidjson::GetValueByPointer(doc, taskJsonPtr) };

            if (!taskValue || !taskValue->IsObject())
            {
                continue;
            }

            auto const taskObject{ taskValue->GetObject() };

            // TODO: Need to find a better way to build each task. Visitor? Busses with Id for
            // each Task?

            rapidjson::Pointer const playResourceKeyPtr{
                JsonKeys::PlayResourceKey_V.Native().data()
            };
            if (auto const* const playResourceValue{
                    rapidjson::GetValueByPointer(taskObject, playResourceKeyPtr) })
            {
                Internal::Task& playTask{ taskDatas.emplace_back(PlaySoundTask{}) };
                AZStd::get<PlaySoundTask>(playTask).m_resourceToPlay =
                    ResourceRef{ playResourceValue->GetString() };

                AZLOG_INFO("Built Task: %s.", taskPath.c_str());
            }

            rapidjson::Pointer const stopResourceKeyPtr{
                JsonKeys::StopEventResourceKey_V.Native().data()
            };
            if (auto const* const stopResourceValue{
                    rapidjson::GetValueByPointer(taskObject, stopResourceKeyPtr) })
            {
                Internal::Task& stopTask{ taskDatas.emplace_back(StopSoundTask{}) };
                AZStd::get<StopSoundTask>(stopTask).m_resourceToStop =
                    ResourceRef{ stopResourceValue->GetString() };
            }
        };

        AudioEvent event{};
        event.m_id =
            AudioEventId{ AZ::IO::Path{ resourceRef.GetCStr() }.Filename().Native().data() };
        event.m_internalData = taskDatas;
        return AZ::Success(event);
    }

    auto AudioEvent::Execute(AudioObject& audioObject) -> AZ::Outcome<void, char const*>
    {
        AZ_Error(
            "AudioEvent",
            m_internalData.is<Internal::TaskContainer>(),
            "The wrong type is stored! It must be a TaskContainer.");

        auto& tasks{ AZStd::any_cast<Internal::TaskContainer&>(m_internalData) };
        for (Internal::Task& task : tasks)
        {
            AZStd::visit(
                [&audioObject](auto&& task)
                {
                    static_assert(
                        AZStd::is_invocable<decltype(task), AudioObject&>::value,
                        "The Task type must be a callable object.");

                    task(audioObject);
                },
                task);
        }

        return AZ::Success();
    }

} // namespace BopAudio
