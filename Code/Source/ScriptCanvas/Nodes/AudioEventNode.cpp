#include "AudioEventNode.h"

#include "AzCore/Console/ILogger.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/AudioEventBus.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    void AudioEventNode::OnDeactivate()
    {
        Nodeable::OnDeactivate();
        DisconnectAudioEvent();
    }

    void AudioEventNode::ConnectAudioEvent(
        AZ::EntityId const owner, AZStd::string const& controlName)
    {
        m_controlId = Audio::AudioStringToID<Audio::TAudioControlID>(controlName.c_str());
        m_controlName = controlName;
        m_owner = owner;

        Audio::AudioTriggerNotificationBus::Handler::BusConnect(m_owner);

        AZLOG(
            LOG_AudioEventNode,
            "An AudioEventNode with [ControlId: %llu | TriggerName: %s] connected.\n",
            m_controlId,
            controlName.c_str());
    }

    void AudioEventNode::DisconnectAudioEvent()
    {
        Audio::AudioTriggerNotificationBus::Handler::BusDisconnect();
    }

    void AudioEventNode::ToggleConnection()
    {
        auto const busIsConnected{ Audio::AudioTriggerNotificationBus::Handler::BusIsConnected() };

        busIsConnected ? Audio::AudioTriggerNotificationBus::Handler::BusDisconnect()
                       : Audio::AudioTriggerNotificationBus::Handler::BusConnect(m_owner);
    }

    void AudioEventNode::ReportDurationInfo(
        [[maybe_unused]] Audio::TAudioControlID triggerId,
        [[maybe_unused]] Audio::TAudioEventID eventId,
        [[maybe_unused]] float duration,
        [[maybe_unused]] float estimatedDuration)
    {
    }

    void AudioEventNode::ReportTriggerStarted(
        [[maybe_unused]] Audio::TAudioControlID incomingControlId)
    {
        AZLOG(
            LOG_AudioEventNode,
            "AudioEventNode [Name: %s | Owner: %lu | ControlId: %llu] has received "
            "ReportTriggerStarted.",
            m_controlName.GetCStr(),
            m_owner.m_owner,
            m_controlId);

        if (m_controlId == incomingControlId)
        {
            AZLOG(
                LOG_AudioEventNode,
                "AudioEventNode [Name: %s | Owner: %lu | ControlId: %llu] is executing the Start "
                "event."
                "event.",
                m_controlName.GetCStr(),
                m_owner.m_owner,
                m_controlId);

            CallStart(StartEventData{});

            AZLOG(
                LOG_AudioEventNode,
                "AudioEventNode [Name: %s | Owner: %lu | ControlId: %llu] has finished executing "
                "the Start event.",
                m_controlName.GetCStr(),
                m_owner.m_owner,
                m_controlId);

            return;
        }

        AZLOG(
            LOG_AudioEventNode,
            "AudioEventNode [Name: %s | Owner: %lu | Id: %llu] received a trigger notification for "
            "control %llu and skipped sending the Start event since it is for a different control.",
            m_controlName.GetCStr(),
            m_owner.m_owner,
            m_controlId,
            incomingControlId);
    }

    void AudioEventNode::ReportTriggerFinished([[maybe_unused]] Audio::TAudioControlID triggerId)
    {
        AZ_Warning("AudioEventNode", false, "Unimplemented.\n");
    }

} // namespace BopAudio::Nodes
