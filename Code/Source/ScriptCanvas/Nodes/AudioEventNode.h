
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Nodeable.h>
#include <ScriptCanvas/Core/NodeableNode.h>

#include <Source/ScriptCanvas/Nodes/AudioEventNode.generated.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/AudioEventBus.h"

namespace BopAudio::Nodes
{
    class AudioEventNode
        : public ScriptCanvas::Nodeable
        , public AudioEventRequestBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(AudioEventNode, AZ::SystemAllocator);
        SCRIPTCANVAS_NODE(AudioEventNode);

    protected:
        void OnDeactivate() override;

        void StartEvent(StartEventData) override;
        void StopEvent(StopEventData) override;
        void SetVolume(double newVolume);
        void MuteAudioEvent();

    private:
        AudioEventId m_targetEvent{};
    };
} // namespace BopAudio::Nodes
