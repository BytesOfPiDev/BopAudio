#include "Engine/AudioObject.h"

#include "AzCore/std/parallel/atomic.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioObject, "AudioObject", "{06041F42-E2FD-49C1-AA4C-04D54416BD9E}");

    AZ_CLASS_ALLOCATOR_IMPL(AudioObject, AZ::SystemAllocator);

    static auto GetNextAudioObjectId() -> AudioObjectId
    {
        static AZStd::atomic_uint32_t NextAudioObjectId{ 2 };
        return AudioObjectId{ NextAudioObjectId++ };
    }

    AudioObject::AudioObject()
        : m_audioObjectId{ GetNextAudioObjectId() }
    {
    }

    void AudioObject::Update(float)
    {
    }

} // namespace BopAudio
