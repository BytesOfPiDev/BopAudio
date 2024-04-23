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

        AudioEventRequestBus::Handler::BusDisconnect();
    }

    // TODO: Try and find a way to allow selecting controls located in the ATl. May have to create a
    // new type.
    void AudioEventNode::ConnectAudioEvent(AZStd::string controlName)
    {
        m_targetEvent = Audio::AudioStringToID<AudioEventId>(controlName.c_str());

        AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_targetEvent });

        AZLOG(
            LOG_AudioEventNode,
            "An AudioEventNode with [ControlId: %u | ControlName: %s] connected.\n",
            static_cast<AZ::u32>(m_targetEvent),
            controlName.c_str());
    }

    void AudioEventNode::DisconnectAudioEvent()
    {
    }

    void AudioEventNode::ToggleConnection()
    {
        auto const busIsConnected{ AudioEventRequestBus::Handler::BusIsConnected() };
        auto const targetEventIsInvalid{ (m_targetEvent == InvalidAudioEventId) };
        auto const shouldDisconnect{ busIsConnected || targetEventIsInvalid };

        shouldDisconnect
            ? AudioEventRequestBus::Handler::BusDisconnect()
            : AudioEventRequestBus::Handler::BusConnect(AudioEventBusIdType{ m_targetEvent });
    }

    void AudioEventNode::StartAudioEvent(StartEventData startData)
    {
        CallStart(startData);
    }

} // namespace BopAudio::Nodes
