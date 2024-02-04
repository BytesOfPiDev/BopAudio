#include "Engine/Tasks/PlaySound.h"

#include "AudioAllocators.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/Tasks/Common.h"
#include "Engine/Tasks/TaskBus.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        PlaySoundTask, "PlayTask", "{4D2E98E1-A6C5-4076-8FE8-556E6CECD77F}");

    AZ_CLASS_ALLOCATOR_IMPL(PlaySoundTask, Audio::AudioImplAllocator);

    void PlaySoundTask::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<PlaySoundTask>()->Version(0)->Field(
                "TargetResource", &PlaySoundTask::m_resourceToPlay);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<PlaySoundTask>("PlaySoundTask", "");
            }
        }
    }

    class PlayTaskFactory : public TaskFactoryBus::Handler
    {
    public:
        AZ_DISABLE_COPY_MOVE(PlayTaskFactory);
        AZ_RTTI(PlayTaskFactory, "{E5F9FFA8-4A86-4711-83DD-C8FC490F66CF}", TaskFactoryRequests);

        PlayTaskFactory()
        {
            BusConnect(AZ_CRC_CE("Play"));
        }

        ~PlayTaskFactory() override = default;

    protected:
        [[nodiscard]] auto Create(AZStd::span<char const> playTaskBuffer) const
            -> AZStd::any override
        {
            PlaySoundTask task{};

            rapidjson::Document doc{};
            doc.Parse(playTaskBuffer.data(), playTaskBuffer.size());

            if (doc.HasParseError())
            {
                AZ_Error("PlayTaskFactory", false, "Failed to parse PlayTask buffer");
                return {};
            }

            auto const playResourcePtr{ rapidjson::Pointer(
                JsonKeys::PlayResourceName.Native().data()) };

            auto* resourceValue{ playResourcePtr.Get(doc) };
            if (!resourceValue)
            {
                AZ_Error(
                    "PlayTaskFactory",
                    false,
                    "Failed to find required key '%s'",
                    JsonKeys::PlayResourceName);

                return {};
            }

            if (!resourceValue->IsString())
            {
                AZ_Error(
                    "PlayTaskFactory",
                    false,
                    "Unexpected type at key '%s' - Expected a string",
                    JsonKeys::PlayResourceName);

                return {};
            }

            task.m_resourceToPlay = ResourceRef{ resourceValue->GetString() };

            return AZStd::any{ Task{ task } };
        };
    };

    auto PlaySoundTask::CreateFactory() -> AZStd::unique_ptr<TaskFactoryRequests>
    {
        return AZStd::make_unique<PlayTaskFactory>();
    }
} // namespace BopAudio
