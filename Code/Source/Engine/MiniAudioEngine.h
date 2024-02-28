#pragma once

#include "AzCore/XML/rapidxml.h"
#include "AzCore/base.h"

#include "BopAudio/Util.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/Sound.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    class AudioSystemImpl_BopAudio;

    class MiniAudioEngine final : public AudioEngineInterface::Registrar
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEngine);

        MiniAudioEngine();
        ~MiniAudioEngine() override;

        [[nodiscard]] auto Initialize() -> AudioOutcome<void> override;
        [[nodiscard]] auto LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
            -> NullOutcome override;
        auto Shutdown() -> bool override;

        auto GetSoundEngine() -> ma_engine* override;

        auto ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest)
            -> AudioOutcome<void> override;

        [[nodiscard]] auto CreateAudioObject(Audio::TAudioObjectID) -> bool override;
        void RemoveAudioObject(Audio::TAudioObjectID) override;

    protected:
        void LoadTrigger(AZ::rapidxml::xml_node<char>*);

        auto FindAudioObject(AudioObjectId audioObjectId) -> AudioObject*;
        [[nodiscard]] auto CreateEvent(AudioEventId resourceId) const
            -> AudioOutcome<AudioEventAsset>;
        void PlaySound(ma_sound* soundInstance, AZ::Name const& soundName);

    private:
        AZStd::unique_ptr<SoundBankAsset> m_initSoundBank{};
        AZStd::vector<AZStd::unique_ptr<SoundBankAsset>> m_soundBanks{};

        AZStd::vector<AudioObject> m_audioObjects{};
        AZStd::unordered_map<UniqueId, SoundInstance> m_soundCache{};
    };
} // namespace BopAudio
