#include "AudioEventNode.h"

#include "AzCore/Console/ILogger.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

/////////////////////////////////////////////////////////////
// This registration only needs to happen once per module
// You can keep it here, or move it into this module's
// system component
#include <ScriptCanvas/AutoGen/ScriptCanvasAutoGenRegistry.h>
#include <Source/AudioEventNode_Nodeables.generated.h>
REGISTER_SCRIPTCANVAS_AUTOGEN_NODEABLE(ScriptCanvasNodesObject);
/////////////////////////////////////////////////////////////

namespace BopAudio::Nodes
{
    void AudioEventNode::OnDeactivate()
    {
        Nodeable::OnDeactivate();

        Audio::AudioTriggerNotificationBus::Handler::BusDisconnect();
    }

    void AudioEventNode::In(AZ::EntityId targetEntity, AZStd::string controlName)
    {
        Audio::AudioTriggerNotificationBus::Handler::BusIsConnected()
            ? Audio::AudioTriggerNotificationBus::Handler::BusDisconnect()
            : void();

        if (!targetEntity.IsValid())
        {
            AZ_Error("AudioEventNode", false, "Invalid target entity");
            return;
        }

        m_controlOwner = targetEntity;

        if (auto audioSystem = AZ::Interface<Audio::IAudioSystem>::Get(); audioSystem != nullptr)
        {
            m_targetControl = audioSystem->GetAudioTriggerID(controlName.c_str());
        }

        if (m_targetControl == INVALID_AUDIO_CONTROL_ID)
        {
            AZ_Error(
                "AudioEventNode",
                false,
                "Failed to find an audio control named '%s'",
                controlName.c_str());

            return;
        }

        Audio::AudioTriggerNotificationBus::Handler::BusConnect(m_controlOwner);

        AZLOG(
            LOG_AudioEventNode,
            "AudioEventNode is connected to entity '%llu'",
            static_cast<AZ::u64>(targetEntity));
    }

    void AudioEventNode::ReportTriggerStarted(Audio::TAudioControlID triggerId)
    {
        if (triggerId == m_targetControl)
        {
            CallEventStarted();
        }
    }

    void AudioEventNode::ReportTriggerFinished(Audio::TAudioControlID triggerId)
    {
        if (triggerId == m_targetControl)
        {
            CallEventFinished();
        }
    }
} // namespace BopAudio::Nodes
