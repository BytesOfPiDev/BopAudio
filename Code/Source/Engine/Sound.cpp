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

    SoundInstance::SoundInstance(ResourceRef const& soundName)
        : m_resourceRef{ soundName.GetCStr() }
        , m_sound{ new ma_sound }
    {
        InitSound();
    };

    void SoundInstance::InitSound()
    {
        AZ_Info("SoundInstance", "InitSound with name: '%s'", m_resourceRef.c_str());
        ma_uint32 const flags = MA_SOUND_FLAG_DECODE;

        ma_result const soundInitResult = ma_sound_init_from_file(
            AudioEngineInterface::Get()->GetSoundEngine(),
            m_resourceRef.c_str(),
            flags,
            nullptr,
            nullptr,
            m_sound.get());

        if (m_isValid = { soundInitResult == MA_SUCCESS }; !m_isValid)
        {
            m_sound = nullptr;
        }
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
