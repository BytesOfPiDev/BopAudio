#pragma once

#include "AzCore/Name/Name.h"
#include "Engine/Id.h"

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

    class Sound
    {
    public:
        AZ_DISABLE_COPY(Sound);

        Sound() = default;
        ~Sound() = default;
        Sound(NamedResource soundName)
            : m_name{ soundName } {};

        [[nodiscard]] auto GetSoundName() const -> NamedResource
        {
            return m_name;
        }

        auto Load() -> bool;

        [[nodiscard]] auto GetData() const -> ma_sound*
        {
            return m_sound.get();
        }
        [[nodiscard]] auto GetData() -> ma_sound*
        {
            return m_sound.get();
        }

    private:
        NamedResource m_name{};
        SoundPtr m_sound{};
    };

} // namespace BopAudio
