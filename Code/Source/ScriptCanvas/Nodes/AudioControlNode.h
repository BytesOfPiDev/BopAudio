
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Nodeable.h>
#include <ScriptCanvas/Core/NodeableNode.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

#include <Source/ScriptCanvas/Nodes/AudioControlNode.generated.h>

#include "Clients/ParameterTypes.h"

namespace BopAudio::Nodes
{
    using namespace ScriptCanvas;

    class AudioControlNode
        : public Nodeable
        , public Audio::AudioTriggerNotificationBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(AudioControlNode, AZ::SystemAllocator);
        SCRIPTCANVAS_NODE(AudioControlNode);

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
        Audio::TriggerNotificationIdType m_ownerId{};
        Audio::TAudioControlID m_controlId{};
        AZ::Name m_controlName{};
    };
} // namespace BopAudio::Nodes
