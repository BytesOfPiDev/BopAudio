#pragma once

#include "AzCore/XML/rapidxml.h"
#include "AzCore/base.h"

#include "BopAudio/Util.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/Sound.h"
#include "Engine/SoundBank.h"

namespace BopAudio
{
    class AudioSystemImpl_BopAudio;

    class MiniAudioEngine final : public AudioEngineInterface::Registrar
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEngine);

        MiniAudioEngine();
        ~MiniAudioEngine() override;

        auto Initialize() -> bool override;
        auto LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool override;
        auto Shutdown() -> bool override;

        auto GetSoundEngine() -> ma_engine* override;

        auto ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest)
            -> AudioOutcome<void> override;

        [[nodiscard]] auto CreateAudioObject(UniqueId const&) -> bool override;
        void RemoveAudioObject(UniqueId audioObjectId) override;

        [[nodiscard]] auto FindSoundBank(ResourceRef const& resourceId) const
            -> AZStd::shared_ptr<rapidjson::Document> override;

    protected:
        void LoadTrigger(AZ::rapidxml::xml_node<char>*);

        auto FindAudioObject(AudioObjectId audioObjectId) -> AudioObject*;
        [[nodiscard]] auto CreateEvent(AudioEventId resourceId) const -> AudioOutcome<AudioEvent>;
        void PlaySound(ma_sound* soundInstance, AZ::Name const& soundName);

    private:
        AZStd::optional<SoundBank> m_initSoundBank{};
        AZStd::vector<SoundBank> m_soundBanks{};

        AZStd::vector<AudioObject> m_audioObjects{};
        AZStd::unordered_map<UniqueId, SoundInstance> m_soundCache{};
    };
} // namespace BopAudio
