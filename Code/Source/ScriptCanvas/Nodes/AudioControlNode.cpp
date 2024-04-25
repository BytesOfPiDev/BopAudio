#include "AudioControlNode.h"

#include "AzCore/Console/ILogger.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/AudioEventBus.h"
#include "Engine/MiniAudioEngineBus.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    void AudioControlNode::OnDeactivate()
    {
        Nodeable::OnDeactivate();
        DisconnectAudioEvent();
    }

    void AudioControlNode::ConnectAudioEvent(
        AZ::EntityId const owner, AZStd::string const& controlName)
    {
        m_controlId = Audio::AudioStringToID<Audio::TAudioControlID>(controlName.c_str());
        m_controlName = controlName;
        m_ownerId = owner;

        Audio::AudioTriggerNotificationBus::Handler::BusConnect(m_ownerId);

        AZLOG(
            LOG_AudioControlNode,
            "An AudioControlNode with [ControlId: %llu | TriggerName: %s] connected.\n",
            m_controlId,
            controlName.c_str());
    }

    void AudioControlNode::DisconnectAudioEvent()
    {
        Audio::AudioTriggerNotificationBus::Handler::BusDisconnect();
    }

    void AudioControlNode::ToggleConnection()
    {
        auto const busIsConnected{ Audio::AudioTriggerNotificationBus::Handler::BusIsConnected() };

        busIsConnected ? Audio::AudioTriggerNotificationBus::Handler::BusDisconnect()
                       : Audio::AudioTriggerNotificationBus::Handler::BusConnect(m_ownerId);
    }

    void AudioControlNode::ReportDurationInfo(
        [[maybe_unused]] Audio::TAudioControlID triggerId,
        [[maybe_unused]] Audio::TAudioEventID eventId,
        [[maybe_unused]] float duration,
        [[maybe_unused]] float estimatedDuration)
    {
        AZLOG(
            LOG_AudioControlNode,
            "AudioControlNode [Name: %s | Owner %lu | ControlId: %llu] received "
            "ReportDurationInfo: [ControlId: %llu | EventId: %llu | Duration: %f | "
            "EstimatedDuration: %f]",
            m_controlName.GetCStr(),
            m_ownerId.m_owner,
            m_controlId,
            triggerId,
            eventId,
            duration,
            estimatedDuration);
    }

    void AudioControlNode::ReportTriggerStarted(
        [[maybe_unused]] Audio::TAudioControlID incomingControlId)
    {
        AZLOG(
            LOG_AudioControlNode,
            "AudioControlNode [Name: %s | Owner: %lu | ControlId: %llu] has received "
            "ReportTriggerStarted.",
            m_controlName.GetCStr(),
            m_ownerId.m_owner,
            m_controlId);

        if (m_controlId == incomingControlId)
        {
            AZLOG(
                LOG_AudioControlNode,
                "AudioControlNode [Name: %s | Owner: %lu | ControlId: %llu] is executing the Start "
                "event."
                "event.",
                m_controlName.GetCStr(),
                m_ownerId.m_owner,
                m_controlId);

            CallStart(StartEventData{});

            AZLOG(
                LOG_AudioControlNode,
                "AudioControlNode [Name: %s | Owner: %lu | ControlId: %llu] has finished executing "
                "the Start event.",
                m_controlName.GetCStr(),
                m_ownerId.m_owner,
                m_controlId);

            return;
        }

        AZLOG(
            LOG_AudioControlNode,
            "AudioControlNode [Name: %s | Owner: %lu | Id: %llu] received a trigger notification for "
            "control %llu and skipped sending the Start event since it is for a different control.",
            m_controlName.GetCStr(),
            m_ownerId.m_owner,
            m_controlId,
            incomingControlId);
    }

    void AudioControlNode::ReportTriggerFinished([[maybe_unused]] Audio::TAudioControlID triggerId)
    {
        AZ_Warning("AudioControlNode", false, "Unimplemented.\n");
    }

} // namespace BopAudio::Nodes
