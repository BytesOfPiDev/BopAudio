#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>

#include "AudioAllocators.h"
#include "AzCore/Serialization/EditContextConstants.inl"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioEventBus.h"
#include "Engine/AudioObject.h"
#include "Engine/Tasks/AudioTaskBase.h"
#include "Engine/Tasks/Common.h"
#include "Engine/Tasks/PlaySound.h"

namespace BopAudio
{
    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        PlaySoundTask::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<IAudioTask>();

            serialize->Class<AudioEventAsset, AZ::Data::AssetData, MiniAudioEventRequests>()
                ->Version(1)
                ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("Tasks", &AudioEventAsset::m_tasks)
                ->Field("EventTasks", &AudioEventAsset::m_eventTasks)
                ->Field("Name", &AudioEventAsset::m_name)
                ->Field("SelectedTask", &AudioEventAsset::m_taskToAdd)
                ->Field("HasSource", &AudioEventAsset::m_hasSource);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("Audio Event Asset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio/Assets")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &AudioEventAsset::m_name, "Name", "")

                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &AudioEventAsset::m_tasks, "Tasks", "")
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify,
                        AZ::Edit::PropertyRefreshLevels::EntireTree)

                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &AudioEventAsset::m_eventTasks,
                        "Event Tasks",
                        "")
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify,
                        AZ::Edit::PropertyRefreshLevels::EntireTree)

                    ->UIElement(AZ::Edit::UIHandlers::Button, "Add Task", "")
                    ->Attribute(AZ::Edit::Attributes::NameLabelOverride, "")
                    ->Attribute(AZ::Edit::Attributes::ButtonText, "Add Task")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &AudioEventAsset::AddTask)
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify,
                        AZ::Edit::PropertyRefreshLevels::EntireTree)

                    ;
            }
        }
    }

    AudioEventAsset::AudioEventAsset() = default;

    AudioEventAsset::~AudioEventAsset()
    {
        UnregisterAudioEvent();
        Cleanup();
    }

    void AudioEventAsset::operator()(AudioObject& audioObject) const
    {
        if (m_eventState != AudioEventState::Idle)
        {
            return;
        }

        AudioEventNotificationBus::Event(
            EventNotificationIdType(Audio::TAudioControlID{}, nullptr),
            &MiniAudioEventNotifications::ReportEventStarted,
            EventStartedData{});

        AZStd::ranges::for_each(
            m_tasks,
            [&audioObject](Task const& taskVariant)
            {
                AZStd::visit(
                    [&audioObject](auto const& task)
                    {
                        if (!task.TryStart(audioObject))
                        {
                            AZ_Error("AudioEventAsset", false, "Failed to start task");
                        }
                    },
                    taskVariant);
            });
    }

    auto AudioEventAsset::TryStartEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != AudioEventState::Idle)
        {
            return false;
        }

        m_eventState = AudioEventState::Active;
        (*this)(obj);

        return true;
    }

    auto AudioEventAsset::TryStopEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != AudioEventState::Active)
        {
            return false;
        }

        m_eventState = AudioEventState::Stopping;
        (*this)(obj);
        m_eventState = AudioEventState::Idle;

        return true;
    }

    void AudioEventAsset::RegisterAudioEvent()
    {
        AZ_Error(
            "AudioEventAsset",
            MiniAudioEventRequestBus::HasHandlers(m_id),
            "Event [%s | %zu] already has a handler!",
            m_name.GetCStr(),
            m_id);

        MiniAudioEventRequestBus::HasHandlers()
            ? MiniAudioEventRequestBus::Handler::BusConnect(m_id)
            : void();
    }

    void AudioEventAsset::UnregisterAudioEvent()
    {
        MiniAudioEventRequestBus::Handler::BusDisconnect();
    };

    void AudioEventAsset::AddTask()
    {
        m_tasks.push_back(PlaySoundTask{});
    }

    void AudioEventAsset::OnTaskSelectionChanged()
    {
    }

    void AudioEventAsset::Cleanup()
    {
        // NOTE: See InputEventGroup::Cleanup()
        AZStd::ranges::for_each(
            m_eventTasks,
            [](IAudioTask* task) -> void
            {
                task->~IAudioTask();
                azfree(static_cast<void*>(task), Audio::AudioImplAllocator);
            });

        m_eventTasks.clear();
    }

    auto AudioEventAsset::GetTaskNames() const -> AZStd::vector<AZStd::string> const
    {
        return { "Play", "Stop" };
    }
} // namespace BopAudio
