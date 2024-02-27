#include "Engine/AudioObject.h"

#include "AudioAllocators.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioObject, "AudioObject", "{06041F42-E2FD-49C1-AA4C-04D54416BD9E}");

    AZ_CLASS_ALLOCATOR_IMPL(AudioObject, Audio::AudioImplAllocator);

    void AudioObject::Update(float)
    {
    }

} // namespace BopAudio
