
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Node.h>
#include <ScriptCanvas/Core/Nodeable.h>

#include <Source/AudioEventNode.generated.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "Clients/AudioEventAsset.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    class AudioEventNode
        : public ScriptCanvas::Nodeable
        , protected Audio::AudioTriggerNotificationBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(AudioEventNode, AZ::SystemAllocator);
        AZ_DISABLE_COPY_MOVE(AudioEventNode);
        SCRIPTCANVAS_NODE(AudioEventNode);

        AudioEventNode() = default;
        ~AudioEventNode() override = default;

    protected:
        void OnDeactivate() override;

        void ReportTriggerStarted(Audio::TAudioControlID triggerId) override;

        void ReportTriggerFinished(Audio::TAudioControlID triggerId) override;

    private:
        Audio::TAudioControlID m_targetControl{};
        Audio::TriggerNotificationIdType m_triggerOwner{};
        AudioEventAssetDataPtr m_eventAsset{};
    };
} // namespace BopAudio::Nodes
