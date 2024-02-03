#include "Engine/MiniAudioEngine.h"

#include "AudioAllocators.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/std/algorithm.h"
#include "Engine/Sound.h"
#include "IAudioSystem.h"
#include "MiniAudio/MiniAudioBus.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/MiniAudioEngineRequests.h"
#include "Engine/MiniAudioIncludes.h"

namespace BopAudio
{
    class AudioSystemImpl_BopAudio;

    MiniAudioEngine::MiniAudioEngine() = default;

    MiniAudioEngine::~MiniAudioEngine()
    {
        AZLOG_INFO("Begin audio object cleanup...");
        m_audioObjects.clear();

        AZLOG_INFO("Finished audio object cleanup.");
    }

    auto MiniAudioEngine::Initialize() -> bool
    {
        m_initSoundBank = SoundBank::LoadInitBank();

        return true;
    }

    auto MiniAudioEngine::Shutdown() -> bool
    {
        return true;
    }

    auto MiniAudioEngine::GetSoundEngine() -> ma_engine*
    {
        return MiniAudio::MiniAudioInterface::Get()->GetSoundEngine();
    }

    auto MiniAudioEngine::LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool
    {
        auto soundBank = SoundBank{ fileEntryInfo };
        if (!soundBank.Load())
        {
            return false;
        }

        m_soundBanks.push_back(soundBank);
        return true;
    }

    void MiniAudioEngine::LoadTrigger(AZ::rapidxml::xml_node<char>* triggerNode)
    {
        if (!triggerNode)
        {
            return;
        }
    }

    auto MiniAudioEngine::ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest) -> bool
    {
        // auto const& triggerId{ activateTriggerRequest.m_triggerId };
        auto const& audioObjectId{ activateTriggerRequest.m_audioObjectId };
        auto const& soundName{ activateTriggerRequest.m_soundName };

        AudioObject* audioObject{ FindAudioObject(audioObjectId) };

        AZ_Warning("MiniAudioEngine", audioObject, "No audio object exists for this trigger!");

        if (audioObject)
        {
            auto iter{ m_soundCache.find(audioObject->m_id) };
            // Create an instance if it's not in the cache.
            if (iter == AZStd::end(m_soundCache))
            {
                auto tmpSoundPtr = CreateSoundByName(soundName);
                auto const& [insertedAtIter, success]{ m_soundCache.insert_or_assign(audioObject->m_id, AZStd::move(tmpSoundPtr)) };
                if (!success)
                {
                    AZ_Error("MiniAudioEngine", false, "Failed to find and create the requested sound.");
                    return false;
                }
                iter = insertedAtIter;
            }

            auto& [instanceId, soundPtr]{ *iter };

            if (ma_sound_is_playing(soundPtr.get()))
            {
                ma_sound_stop(soundPtr.get());
            }
        }

        auto soundPtr = AZStd::move(CreateSoundByName(soundName));
        if (!soundPtr)
        {
            AZ_Error("MiniAudioEngine", false, "Failed to create sound: '%s'.", soundName.GetCStr());
            return false;
        }

        ma_sound_set_looping(soundPtr.get(), false);
        PlaySound(soundPtr.get(), soundName);
        activateTriggerRequest.m_eventData->m_soundInstance = AZStd::move(soundPtr);

        return true;
    }

    auto MiniAudioEngine::ActivateTrigger([[maybe_unused]] BA_TriggerId triggerId) -> bool
    {
        if (m_soundCache.empty())
        {
            return false;
        }

        auto iter{ m_soundCache.begin() };
        auto& [soundName, soundPtr]{ *iter };

        ma_sound_seek_to_pcm_frame(soundPtr.get(), 0);
        ma_sound_start(soundPtr.get());

        return true;
    }

    auto MiniAudioEngine::CreateAudioObject(SATLAudioObjectData_BopAudio* const audioObjectData) -> BA_GameObjectId
    {
        return audioObjectData ? m_audioObjects.emplace_back(audioObjectData->m_name.GetStringView()).GetUniqueId() : InvalidBaUniqueId;
    }

    void MiniAudioEngine::RemoveAudioObject(BA_UniqueId audioObjectId)
    {
        AZStd::remove_if(
            m_audioObjects.begin(),
            m_audioObjects.end(),
            [audioObjectId](auto const& audioObject) -> bool
            {
                return audioObject.GetUniqueId() == audioObjectId;
            });
    }

    auto MiniAudioEngine::FindAudioObject(BA_UniqueId audioObjectId) -> AudioObject*
    {
        auto iter = AZStd::ranges::find_if(
            m_audioObjects,
            [&audioObjectId](auto const& audioObject) -> bool
            {
                return audioObjectId == audioObject.GetUniqueId();
            });

        return iter != AZStd::end(m_audioObjects) ? &(*iter) : nullptr;
    }

    void MiniAudioEngine::PlaySound(ma_sound* soundInstance, AZ::Name const& soundName)
    {
        auto result = ma_sound_seek_to_pcm_frame(soundInstance, 0);

        AZ_Error("MiniAudioEngine", result == MA_SUCCESS, "Failed to seek audio frame on sound: '%s'.", soundName.GetCStr());

        result = ma_sound_start(soundInstance);

        AZ_Error("MiniAudioEngine", result == MA_SUCCESS, "Failed to start sound: '%s'.", soundName.GetCStr());
    }
} // namespace BopAudio
