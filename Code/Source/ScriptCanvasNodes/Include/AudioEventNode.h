
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Node.h>
#include <ScriptCanvas/Core/Nodeable.h>

#include <Source/AudioEventNode.generated.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    class AudioEventNode
        : public ScriptCanvas::Nodeable
        , protected Audio::AudioTriggerNotificationBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(AudioEventNode, AZ::SystemAllocator);
        SCRIPTCANVAS_NODE(AudioEventNode);

    protected:
        void OnDeactivate() override;

        void ReportTriggerStarted(Audio::TAudioControlID triggerId) override;

        void ReportTriggerFinished(Audio::TAudioControlID triggerId) override;

    private:
        Audio::TAudioControlID m_targetControl{};
        Audio::TriggerNotificationIdType m_controlOwner{};
    };
} // namespace BopAudio::Nodes
