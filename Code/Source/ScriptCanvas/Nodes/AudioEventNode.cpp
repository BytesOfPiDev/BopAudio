#include "AudioEventNode.h"

#include "AzCore/Console/ILogger.h"
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

    void AudioEventNode::ConnectAudioEvent(AZStd::string const triggerName)
    {
        m_eventId = Audio::AudioStringToID<AudioEventId>(triggerName.c_str());

        AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_eventId });

        AZLOG(
            LOG_AudioEventNode,
            "An AudioEventNode with [TriggerId: %llu | TriggerName: %s] connected.\n",
            aznumeric_cast<Audio::TAudioEventID>(m_eventId),
            triggerName.c_str());
    }

    void AudioEventNode::DisconnectAudioEvent()
    {
        AudioEventRequestBus::Handler::BusDisconnect();
    }

    void AudioEventNode::ToggleConnection()
    {
        auto const busIsConnected{ AudioEventRequestBus::Handler::BusIsConnected() };

        busIsConnected
            ? AudioEventRequestBus::Handler::BusDisconnect()
            : AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_eventId });
    }

    void AudioEventNode::StartAudioEvent(StartEventData startData)
    {
        CallStart(startData);
    }

} // namespace BopAudio::Nodes
