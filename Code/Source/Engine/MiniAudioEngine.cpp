#include "Engine/MiniAudioEngine.h"

#include "AudioAllocators.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/std/algorithm.h"
#include "IAudioSystem.h"
#include "MiniAudio/MiniAudioBus.h"

#include "Engine/ATLEntities_BopAudio.h"
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
        return true;
    }

    auto MiniAudioEngine::Shutdown() -> bool
    {
        return true;
    }

    auto MiniAudioEngine::LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool
    {
        m_soundBanks.push_back(fileEntryInfo);

        auto& soundBank = m_soundBanks.back();
        soundBank.Load();

        return true;
    }

    void MiniAudioEngine::LoadTrigger(AZ::rapidxml::xml_node<char>* triggerNode)
    {
        if (!triggerNode)
        {
            return;
        }
    }

    auto MiniAudioEngine::ActivateTrigger([[maybe_unused]] BA_TriggerId triggerId) -> bool
    {
        if (m_triggerSounds.empty())
        {
            return false;
        }

        auto iter{ m_triggerSounds.begin() };
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

    void MiniAudioEngine::LoadEventsFolder()
    {
    }

} // namespace BopAudio
