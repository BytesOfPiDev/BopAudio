#include "Engine/Sound.h"

#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudioIncludes.h"

namespace BopAudio
{
    void SoundDeleter::operator()(ma_sound* ptr)
    {
        ma_sound_stop(ptr);
        ma_sound_uninit(ptr);
    }

    SoundInstance::SoundInstance(SoundRef const& soundName)
        : m_resourceRef{ soundName }
        , m_sound{ new ma_sound }
        , m_volume{ 1.0f }
    {
        if (!soundName.IsValid())
        {
            m_sound = nullptr;
        }

        ma_uint32 const flags = MA_SOUND_FLAG_DECODE;

        ma_result const soundInitResult = ma_sound_init_from_file(
            SoundEngine::Get()->GetSoundEngine(),
            m_resourceRef.c_str(),
            flags,
            nullptr,
            nullptr,
            m_sound.get());

        if (m_isValid = { soundInitResult == MA_SUCCESS }; !m_isValid)
        {
            m_sound = nullptr;
        }
    };

    SoundInstance::SoundInstance(SoundSource const&)
    {
    }

    SoundInstance::SoundInstance(SoundInstance const& other)
        : m_resourceRef{ AZStd::move(other.m_resourceRef) }
        , m_sound{ new ma_sound }
        , m_volume{ other.m_volume }
        , m_isValid{ other.m_isValid }
    {
        auto* const engine{ ma_sound_get_engine(other.m_sound.get()) };
        ma_sound_flags flags{ MA_SOUND_FLAG_DECODE };
        ma_result initResult{ ma_sound_init_copy(
            engine, other.m_sound.get(), flags, nullptr, m_sound.get()) };

        m_isValid = { initResult == MA_SUCCESS };
    }

    SoundInstance::SoundInstance(SoundInstance&& other)
        : m_resourceRef{ AZStd::move(other.m_resourceRef) }
        , m_sound(AZStd::move(other.m_sound))
        , m_volume{ other.m_volume }
        , m_isValid{ other.m_isValid }
    {
        other.m_isValid = false;
    }

    auto SoundInstance::operator=(SoundInstance const& other) -> SoundInstance&
    {
        auto* engine{ ma_sound_get_engine(other.m_sound.get()) };
        m_sound.reset(new ma_sound);

        ma_sound_flags flags{ MA_SOUND_FLAG_DECODE };
        ma_sound_init_copy(engine, other.m_sound.get(), flags, nullptr, m_sound.get());

        m_resourceRef = other.m_resourceRef;
        m_volume = other.m_volume;
        m_isValid = other.m_isValid;

        return *this;
    }

    auto SoundInstance::operator=(SoundInstance&& other) -> SoundInstance&
    {
        m_resourceRef = AZStd::move(other.m_resourceRef);
        m_sound = AZStd::move(other.m_sound);
        m_volume = other.m_volume;
        m_isValid = other.m_isValid;

        other.m_isValid = false;

        return *this;
    }

    void SoundInstance::Play(int beginPcmFrame)
    {
        AZ_Warning("SoundInstance", false, "Got Play");

        if (!m_isValid)
        {
            AZ_Error("SoundInstance", false, "Cannot play invalid sound instance");
            return;
        }

        ma_sound_seek_to_pcm_frame(m_sound.get(), beginPcmFrame);
        ma_sound_set_volume(m_sound.get(), m_volume);
        ma_sound_start(m_sound.get());
    }

} // namespace BopAudio
