#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/EditContextConstants.inl"
#include "IAudioInterfacesCommonData.h"

#include "Engine/AudioObject.h"
#include "Engine/Tasks/AudioTaskBase.h"
#include "Engine/Tasks/PlaySound.h"

namespace BopAudio
{
    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            PlaySoundTask::Reflect(context);

            serialize->Class<IAudioTask>()->Version(0);
            serialize->RegisterGenericType<decltype(AudioEventAsset::m_eventTasks)>();

            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()
                ->Version(4)
                ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("EventTasks", &AudioEventAsset::m_eventTasks)
                ->Field("Name", &AudioEventAsset::m_name);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("Audio Event Asset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio/Assets")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &AudioEventAsset::m_name, "Name", "")

                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify,
                        AZ::Edit::PropertyRefreshLevels::EntireTree)

                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &AudioEventAsset::m_eventTasks,
                        "Event Tasks",
                        "")
                    ->Attribute(AZ::Edit::Attributes::ContainerReorderAllow, true)
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify,
                        AZ::Edit::PropertyRefreshLevels::AttributesAndValues);
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
        if (m_eventState != Audio::EAudioEventState::eAES_NONE)
        {
            return;
        }

        AZStd::ranges::for_each(
            m_eventTasks,
            [&audioObject](IAudioTask* audioTask)
            {
                if (audioTask)
                {
                    (*audioTask)(audioObject);
                }
            });
    }

    auto AudioEventAsset::TryStartEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != Audio::EAudioEventState::eAES_NONE)
        {
            return false;
        }

        m_eventState = Audio::EAudioEventState::eAES_NONE;
        (*this)(obj);

        return true;
    }

    auto AudioEventAsset::TryStopEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != Audio::EAudioEventState::eAES_PLAYING)
        {
            return false;
        }

        m_eventState = Audio::EAudioEventState::eAES_UNLOADING;
        (*this)(obj);
        m_eventState = Audio::EAudioEventState::eAES_NONE;

        return true;
    }

    void AudioEventAsset::RegisterAudioEvent()
    {
        if (m_id == InvalidAudioEventId)
        {
            AZ_Error(
                "AudioEventAsset", false, "Unable to register audio event. The id is invalid.\n");

            return;
        }

        if (MiniAudioEventRequestBus::HasHandlers(m_id))
        {
            AZ_Error(
                "AudioEventAsset",
                false,
                "Unable to register audio event. Handler already exists [%s | %zu].\n",
                m_name.GetCStr(),
                m_id);

            return;
        }

        auto const alreadyConnected{ MiniAudioEventRequestBus::Handler::BusIsConnected() };
        !alreadyConnected ? MiniAudioEventRequestBus::Handler::BusConnect(m_id) : void();
    }

    void AudioEventAsset::UnregisterAudioEvent()
    {
        MiniAudioEventRequestBus::Handler::BusDisconnect();
    };

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
                azfree(static_cast<void*>(task), AZ::SystemAllocator);
            });

        m_eventTasks.clear();
    }

    auto AudioEventAsset::GetTaskNames() const -> AZStd::vector<AZStd::string> const
    {
        return { "Play", "Stop" };
    }

    void StartEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StartEventData>()->Version(0);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StartEventData>("Start AudioEvent Data", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<StartEventData>("StartAudioEventData");
        }
    }

    void StopEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StopEventData>()->Version(0);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StopEventData>("Stop AudioEvent Data", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<StopEventData>("StopAudioEventData");
        }
    }

} // namespace BopAudio
