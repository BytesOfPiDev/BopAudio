#include "Engine/AudioEvent.h"

#include "AzCore/Memory/SystemAllocator.h"

#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(AudioEvent, "AudioEvent", "{5E28282D-485B-4B1B-9F8C-8AC8C4A77340}");
    AZ_CLASS_ALLOCATOR_IMPL(AudioEvent, AZ::SystemAllocator);

    auto TryLoadEventAsset(AZ::Data::Asset<AudioEventAsset> eventAsset) -> AudioEventAsset*
    {
        if (!eventAsset.QueueLoad())
        {
            return nullptr;
        }

        eventAsset.BlockUntilLoadComplete();
        return eventAsset.Get();
    }

    void AudioEvent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEvent>()->Version(0);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEvent>("AudioEvent", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    AudioEvent::AudioEvent(AZ::Data::Asset<AudioEventAsset> eventAsset)
        : m_eventAsset{ AZStd::move(eventAsset) }
    {
    }
} // namespace BopAudio
