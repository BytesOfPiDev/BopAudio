#include "Engine/MiniAudioEngine.h"

#include "AzCore/Console/ILogger.h"
#include "AzCore/JSON/schema.h"
#include "Engine/AudioObject.h"
#include "Engine/Common_BopAudio.h"
#include "MiniAudio/MiniAudioBus.h"

#include "Engine/AudioEvent.h"
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
        auto loadBankOutcome{ SoundBank::LoadInitBank() };
        m_initSoundBank = loadBankOutcome ? loadBankOutcome.TakeValue() : nullptr;
        if (!m_initSoundBank.has_value())
        {
            AZLOG_ERROR(
                "The MiniAudioEngine failed to load the default soundbank. Reason: %s.",
                loadBankOutcome.GetError());

            return false;
        }

        return true;
    }

    auto MiniAudioEngine::Shutdown() -> bool
    {
        m_initSoundBank.reset();
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

    auto MiniAudioEngine::ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest)
        -> AudioEventId
    {
        auto const audioObjectId{ activateTriggerRequest.m_audioObjectId };
        auto* audioObject{ FindAudioObject(audioObjectId) };

        if (!audioObject)
        {
            auto& newAudioObject = m_audioObjects.emplace_back();
            audioObject = &newAudioObject;
        }

        return {};
    }

    /*
        auto MiniAudioEngine::ActivateTrigger(ResourceId, UniqueId) -> bool
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
        */

    auto MiniAudioEngine::CreateAudioObject(UniqueId const&) -> bool
    {
        return true;
    }

    void MiniAudioEngine::RemoveAudioObject(UniqueId /*audioObjectId*/)
    {
        /*
              AZStd::remove_if(
                  m_audioObjects.begin(),
                  m_audioObjects.end(),
                  [audioObjectId](auto const& audioObject) -> bool
                  {
                      return audioObject == audioObjectId;
                  });
        */
    }

    auto MiniAudioEngine::FindSoundBank(NamedResource const& resourceId) const
        -> AZStd::shared_ptr<rapidjson::Document>
    {
        auto const foundIter = AZStd::ranges::find_if(
            m_soundBanks,
            [&resourceId](SoundBank const& soundBank)
            {
                return soundBank == resourceId;
            });

        return foundIter != AZStd::end(m_soundBanks) ? foundIter->GetDocument() : nullptr;
    }

    auto MiniAudioEngine::FindAudioObject(AudioObjectId /*audioObjectId*/) -> AudioObject*
    {
        /*
              auto iter = AZStd::ranges::find_if(
                  m_audioObjects,
                  [&audioObjectId](auto const& audioObject) -> bool
                  {
                      return audioObjectId == audioObject.GetUniqueId();
                  });

              return iter != AZStd::end(m_audioObjects) ? &(*iter) : nullptr;
        */
        return nullptr;
    }

    void MiniAudioEngine::PlaySound(ma_sound* soundInstance, AZ::Name const& soundName)
    {
        auto result = ma_sound_seek_to_pcm_frame(soundInstance, 0);

        AZ_Error(
            "MiniAudioEngine",
            result == MA_SUCCESS,
            "Failed to seek audio frame on sound: '%s'.",
            soundName.GetCStr());

        result = ma_sound_start(soundInstance);

        AZ_Error(
            "MiniAudioEngine",
            result == MA_SUCCESS,
            "Failed to start sound: '%s'.",
            soundName.GetCStr());
    }
} // namespace BopAudio
