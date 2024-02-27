#include "Engine/AudioEvent.h"
#include "AudioAllocators.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(AudioEvent, "AudioEvent", "{5E28282D-485B-4B1B-9F8C-8AC8C4A77340}");
    AZ_CLASS_ALLOCATOR_IMPL(AudioEvent, Audio::AudioImplAllocator);

    void AudioEvent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEvent>()
                ->Version(0)
                ->Field("Id", &AudioEvent::m_id)
                ->Field("Data", &AudioEvent::m_internalData);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEvent>("AudioEvent", "");
            }
        }
    }

    AudioEvent::AudioEvent(AZ::Data::Asset<AudioEventAsset> eventAsset)
        : m_eventAsset{ AZStd::move(eventAsset) }
    {
    }
} // namespace BopAudio
