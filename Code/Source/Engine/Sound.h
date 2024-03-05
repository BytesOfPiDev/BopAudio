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
        AZ_DISABLE_COPY(SoundInstance);

        SoundInstance() = default;
        ~SoundInstance() = default;
        SoundInstance(ResourceRef const& soundName);

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
        AZStd::string m_resourceRef{};
        SoundPtr m_sound{};
        bool m_isValid{};

        float m_volume{};
    };

} // namespace BopAudio
