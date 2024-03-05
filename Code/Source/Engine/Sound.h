#pragma once

#include "Engine/Id.h"
#include "Engine/MiniAudioIncludes.h"

struct ma_sound;

namespace BopAudio
{
    struct SoundDeleter
    {
        AZ_DISABLE_COPY(SoundDeleter);

        SoundDeleter() = default;
        ~SoundDeleter() = default;

        void operator()(ma_sound* ptr);
    };

    using SoundPtr = std::unique_ptr<ma_sound, SoundDeleter>;

    class SoundInstance
    {
    public:
        SoundInstance() = default;
        ~SoundInstance() = default;

        SoundInstance(SoundRef const& soundName);

        SoundInstance(SoundInstance const&);
        SoundInstance(SoundInstance&&);

        auto operator=(SoundInstance const&) -> SoundInstance&;
        auto operator=(SoundInstance&&) -> SoundInstance&;

        [[nodiscard]] constexpr auto IsValid() const
        {
            return m_isValid;
        }

        void Play(int beginPcmFrame = 0);

        constexpr void SetVolume(float volume)
        {
            m_volume = volume;
        }

    protected:
        void InitSound();

    private:
        SoundRef m_resourceRef{};
        SoundPtr m_sound{};
        bool m_isValid{};

        float m_volume{};
    };

} // namespace BopAudio
