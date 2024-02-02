#pragma once

#include "AzCore/Name/Name.h"

struct ma_sound;

namespace BopAudio
{

    struct SoundDeleter
    {
        AZ_DISABLE_COPY(SoundDeleter);

        SoundDeleter() = default;
        SoundDeleter(AZ::Name soundName)
            : m_soundName(AZStd::move(soundName)){};
        ~SoundDeleter() = default;

        void operator()(ma_sound* ptr);

        AZ::Name m_soundName{};
    };

    using SoundPtr = std::unique_ptr<ma_sound, SoundDeleter>;

    struct Sound
    {
        AZ::Name m_name;
        SoundPtr m_sound;
    };

    auto CreateSoundByName(AZ::Name const& soundName) -> SoundPtr;
} // namespace BopAudio
