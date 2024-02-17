#include "Engine/MiniAudioEngine.h"

#include "AzCore/Console/ILogger.h"
#include "MiniAudio/MiniAudioBus.h"

#include "BopAudio/Util.h"
#include "Engine/AudioEvent.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"
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
        -> AudioOutcome<void>
    {
        // We need an audio object to activate the trigger/event on. The request should have
        // provided one.
        auto* audioObject{ FindAudioObject(activateTriggerRequest.m_audioObjectId) };
        // For now, we'll create the audio object if it doesn't already exist, but emit a warning.
        if (!audioObject)
        {
            AZ_Warning(
                "MiniAudioEngine",
                false,
                "The given audio object does not exist, so we had to create it.");

            auto& newAudioObject = m_audioObjects.emplace_back();
            audioObject = &newAudioObject;
        }

        if (auto createEventOutcome{ CreateEvent(activateTriggerRequest.m_triggerResource) })
        {
            audioObject->AddEvent(createEventOutcome.TakeValue());
        }
        else
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to create event with resource ['%s'].",
                activateTriggerRequest.m_triggerResource.GetCStr()));
        }

        return AZ::Success();
    }

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

    auto MiniAudioEngine::FindSoundBank(ResourceRef const& resourceId) const
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

    auto MiniAudioEngine::CreateEvent(AudioEventId eventId) const -> AudioOutcome<AudioEvent>
    {
        AudioOutcome<AudioEvent> createEventOutcome{};
        AZStd::ranges::find_if(
            m_soundBanks,
            [&eventId, &createEventOutcome](auto const& soundBank)
            {
                AZ_Info(
                    "MiniAudioEngine",
                    "CreateEvent checking sound bank: [%s]",
                    soundBank.GetResourceId().GetCStr());
                createEventOutcome = soundBank.CloneEvent(eventId);
                return createEventOutcome.IsSuccess();
            });

        return createEventOutcome.IsSuccess()
            ? createEventOutcome
            : AZ::Failure("CreateEvent failed. Reason: ['%s']. Failed to find an event with the "
                          "given resource id.");
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
