#include "Engine/Sound.h"

#include "AzCore/IO/Path/Path.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudioIncludes.h"
#include <AzCore/IO/Path/Path_fwd.h>

namespace BopAudio
{
    void SoundDeleter::operator()(ma_sound* ptr)
    {
        ma_sound_stop(ptr);
        ma_sound_uninit(ptr);
        ma_resource_manager_unregister_data(
            ma_engine_get_resource_manager(MiniAudio::MiniAudioInterface::Get()->GetSoundEngine()),
            m_soundName.GetCStr());
    }

    auto CreateSoundByName(AZ::Name const& soundName) -> SoundPtr
    {
        ma_engine* engine{ AudioEngineInterface::Get()->GetSoundEngine() };
        AZ_Error("CreateSoundByName", engine != nullptr, "Failed to get miniaudio sound engine!");

        // Try and load an instance of the sound.
        SoundPtr soundPtr = [&soundName]() -> decltype(soundPtr)
        {
            auto const o3deProjectRelativePath{ AZ::IO::Path{ GetBanksRootPath() } /
                                                soundName.GetStringView() };
            AZ::Name const resourceName{ o3deProjectRelativePath.Native() };

            decltype(soundPtr) tempPtr{ new ma_sound, SoundDeleter(resourceName) };

            ma_uint32 const flags = MA_SOUND_FLAG_DECODE;
            ma_result result = ma_sound_init_from_file(
                AudioEngineInterface::Get()->GetSoundEngine(),
                resourceName.GetCStr(),
                flags,
                nullptr,
                nullptr,
                tempPtr.get());

            AZ_Error(
                "CreateSoundByName",
                result == MA_SUCCESS,
                "Failed to init sound using resource name: %s.",
                resourceName.GetCStr());

            return AZStd::move(tempPtr);
        }();

        if (soundPtr == nullptr)
        {
            return nullptr;
        }

        ma_sound_set_volume(soundPtr.get(), 1.0f);
        ma_sound_set_looping(soundPtr.get(), true);

        return AZStd::move(soundPtr);
    }
} // namespace BopAudio
