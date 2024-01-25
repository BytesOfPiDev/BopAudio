#pragma once

#include "AzCore/Name/Name.h"

struct ma_sound;
struct ma_engine;

namespace BopAudio
{

    struct SoundDeleter
    {
        AZ_DEFAULT_COPY_MOVE(SoundDeleter);

        SoundDeleter() = default;
        SoundDeleter(AZ::Name soundName, ma_engine* engine)
            : m_soundName(AZStd::move(soundName))
            , m_engine(engine){};
        ~SoundDeleter() = default;

        void operator()(ma_sound* ptr);

        AZ::Name m_soundName{};
        ma_engine* m_engine{};
    };

    using SoundPtr = std::unique_ptr<ma_sound, SoundDeleter>;

    struct Sound
    {
        AZ::Name m_name;
        SoundPtr m_sound;
    };
} // namespace BopAudio
