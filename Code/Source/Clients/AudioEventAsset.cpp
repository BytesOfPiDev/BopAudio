#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/Outcome/Outcome.h>

#include "AudioAllocators.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/Tasks/Common.h"

namespace BopAudio
{

    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAsset, Audio::AudioImplAllocator);

    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAsset, AZ::Data::AssetData);

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAsset, "AudioEventAsset", "{F26CEC71-D882-4367-BCBF-B9B041E1C708}");

    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()
                ->Version(0)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("Tasks", &AudioEventAsset::m_tasks);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("AudioEventAsset", "");
            }
        }
    }

    auto AudioEventAsset::Execute(AudioObject& audioObject) const -> AZ::Outcome<void, char const*>
    {
        if (m_eventState != AudioEventState::Idle)
        {
            return AZ::Failure("Not in Idle state");
        }

        AudioEventNotificationBus::Event(
            EventNotificationIdType(Audio::TAudioControlID{}, nullptr),
            &AudioEventNotifications::ReportEventStarted,
            EventStartedData{});

        AZStd::ranges::for_each(
            m_tasks,
            [&audioObject](Task const& taskVariant)
            {
                AZStd::visit(
                    [&audioObject](auto const& task)
                    {
                        if (!task.TryStart(audioObject))
                        {
                            AZ_Error("AudioEventAsset", false, "Failed to start task");
                        };
                    },
                    taskVariant);
            });

        return AZ::Success();
    }

    void AudioEventAsset::operator()(AudioObject& audioObject) const
    {
        Execute(audioObject);
    }

} // namespace BopAudio
