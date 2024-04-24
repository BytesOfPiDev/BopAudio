#include "AudioEventNode.h"

#include "AzCore/Console/ILogger.h"
#include "Clients/AudioEventBus.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    void AudioEventNode::OnDeactivate()
    {
        Nodeable::OnDeactivate();
        DisconnectAudioEvent();
    }

    void AudioEventNode::ConnectAudioEvent(AZStd::string triggerName)
    {
        m_targetEvent = Audio::AudioStringToID<AudioEventId>(triggerName.c_str());

        AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_targetEvent });

        AZLOG(
            LOG_AudioEventNode,
            "An AudioEventNode with [TriggerId: %u | TriggerName: %s] connected.\n",
            static_cast<AZ::u32>(m_targetEvent),
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
            : AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_targetEvent });
    }

    void AudioEventNode::StartAudioEvent(StartEventData startData)
    {
        CallStart(startData);
    }

} // namespace BopAudio::Nodes
