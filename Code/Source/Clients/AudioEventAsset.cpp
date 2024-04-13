#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>

#include "AudioAllocators.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/Serialization/EditContextConstants.inl"

#include "BopAudio/BopAudioTypeIds.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioEventBus.h"
#include "Engine/AudioObject.h"
#include "Engine/Tasks/Common.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAsset, Audio::AudioImplAllocator);
    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAsset, AZ::Data::AssetData, MiniAudioEventRequests);
    AZ_TYPE_INFO_WITH_NAME_IMPL(AudioEventAsset, "AudioEventAsset", AudioEventAssetTypeId);

    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        PlaySoundTask::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData, MiniAudioEventRequests>()
                ->Version(1)
                ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("Tasks", &AudioEventAsset::m_tasks);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("Audio Event Asset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &AudioEventAsset::m_id, "Id", "");
            }
        }
    }

    AudioEventAsset::AudioEventAsset() = default;

    AudioEventAsset::~AudioEventAsset()
    {
        UnregisterAudioEvent();
    }

    void AudioEventAsset::operator()(AudioObject& audioObject) const
    {
        if (m_eventState != AudioEventState::Idle)
        {
            return;
        }

        AudioEventNotificationBus::Event(
            EventNotificationIdType(Audio::TAudioControlID{}, nullptr),
            &MiniAudioEventNotifications::ReportEventStarted,
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
    }

    auto AudioEventAsset::TryStartEvent(AudioObject& obj) -> bool
    {
        AZLOG(LOG_AudioEventAsset, "Received TryStartEvent");
        auto const& self(*this);
        self(obj);

        return true;
    }

    auto AudioEventAsset::TryStopEvent(AudioObject& obj) -> bool
    {
        AZ_Error("AudioEventAsset", false, "TryStopEvent");
        auto const& self(*this);
        self(obj);

        return true;
    }

    void AudioEventAsset::RegisterAudioEvent()
    {
        MiniAudioEventRequestBus::Handler::BusConnect(m_id);
    }

    void AudioEventAsset::UnregisterAudioEvent()
    {
        MiniAudioEventRequestBus::Handler::BusDisconnect();
    };

} // namespace BopAudio
