
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Nodeable.h>
#include <ScriptCanvas/Core/NodeableNode.h>

#include <Source/ScriptCanvas/Nodes/AudioEventNode.generated.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/AudioEventBus.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

namespace BopAudio::Nodes
{
    using namespace ScriptCanvas;

    class AudioEventNode
        : public Nodeable
        , public Audio::AudioTriggerNotificationBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(AudioEventNode, AZ::SystemAllocator);
        SCRIPTCANVAS_NODE(AudioEventNode);

    protected:
        void OnDeactivate() override;

        void ReportDurationInfo(
            Audio::TAudioControlID triggerId,
            Audio::TAudioEventID eventId,
            float duration,
            float estimatedDuration) override;

        void ReportTriggerStarted([[maybe_unused]] Audio::TAudioControlID controlId) override;
        void ReportTriggerFinished([[maybe_unused]] Audio::TAudioControlID controlId) override;

    private:
        Audio::TriggerNotificationIdType m_owner{};
        Audio::TAudioControlID m_controlId{};
        AZ::Name m_controlName{};
    };
} // namespace BopAudio::Nodes
