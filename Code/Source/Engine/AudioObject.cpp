#include "Engine/AudioObject.h"

#include "AudioAllocators.h"
#include "AzCore/std/parallel/atomic.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioObject, "AudioObject", "{06041F42-E2FD-49C1-AA4C-04D54416BD9E}");

    AZ_CLASS_ALLOCATOR_IMPL(AudioObject, Audio::AudioImplAllocator);

    static auto GetNextAudioObjectId() -> AudioObjectId
    {
        static AZStd::atomic_uint32_t NextAudioObjectId{ 2 };
        return NextAudioObjectId++;
    }

    AudioObject::AudioObject()
        : m_audioObjectId{ GetNextAudioObjectId() }
    {
    }

    void AudioObject::Update(float)
    {
    }

    void AudioObject::PlaySound(SoundInstance&& soundInstance)
    {
        m_sound = AZStd::move(soundInstance);
        m_sound.Play();
    }

} // namespace BopAudio
