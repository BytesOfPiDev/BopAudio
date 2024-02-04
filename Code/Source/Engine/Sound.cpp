#include "Engine/Sound.h"

#include "AzCore/IO/Path/Path.h"
#include "AzCore/IO/Path/Path_fwd.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudio/MiniAudioBus.h"
#include "MiniAudioIncludes.h"

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

    auto CreateSoundByName(ResourceRef const& soundName) -> SoundPtr
    {
        ma_engine* engine{ AudioEngineInterface::Get()->GetSoundEngine() };
        AZ_Error("CreateSoundByName", engine != nullptr, "Failed to get miniaudio sound engine!");

        SoundPtr soundPtr = [&soundName]() -> decltype(soundPtr)
        {
            // WARNING: We expected the sound name to be the sound asset's path relative to the
            // soundbank directory. Anything else will result in failure.
            // TODO: Implement finding by numeric Id / hash
            auto const o3deProjectRelativePath{ AZ::IO::Path{ GetBanksRootPath() } /
                                                soundName.GetCStr() };

            AZ::Name const resourceName{ o3deProjectRelativePath.Native() };

            decltype(soundPtr) tempPtr{ new ma_sound, SoundDeleter(resourceName) };

            ma_uint32 const flags = MA_SOUND_FLAG_DECODE;
            // We expect the sound to *already* have been registered with the above resourceName.
            ma_result result = ma_sound_init_from_file(
                AudioEngineInterface::Get()->GetSoundEngine(),
                resourceName.GetCStr(),
                flags,
                nullptr,
                nullptr,
                tempPtr.get());

            // HACK: For the time being, we'll try the sound name if the full project path from
            // resourceName did not work.
            if (result != MA_SUCCESS)
            {
                result = ma_sound_init_from_file(
                    AudioEngineInterface::Get()->GetSoundEngine(),
                    soundName.GetCStr(),
                    flags,
                    nullptr,
                    nullptr,
                    tempPtr.get());

                AZ_Warning(
                    "CreateSoundByName",
                    result == MA_SUCCESS,
                    "Found the sound by the short resource name instead of the computed full "
                    "resource name.");
            }

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

    auto SoundInstance::Load() -> bool
    {
        m_sound = CreateSoundByName(m_name);
        return m_sound != nullptr;
    }
} // namespace BopAudio
