#include "Engine/Sound.h"

#include "MiniAudioIncludes.h"

namespace BopAudio
{
    void SoundDeleter::operator()(ma_sound* ptr)
    {
        ma_sound_stop(ptr);
        ma_sound_uninit(ptr);
        ma_resource_manager_unregister_data(ma_engine_get_resource_manager(m_engine), m_soundName.GetCStr());
    }

    auto MakeSoundPtr(AZ::Name const& soundName, ma_engine* engine) -> SoundPtr
    {
        return { new ma_sound, SoundDeleter(soundName, engine) };
    }
} // namespace BopAudio
