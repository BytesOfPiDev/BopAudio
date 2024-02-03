#pragma once

#include "AzCore/XML/rapidxml.h"
#include "AzCore/base.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/MiniAudioEngineBus.h"
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

        auto ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest) -> bool override;
        auto ActivateTrigger(BA_TriggerId baId) -> bool;

        [[nodiscard]] auto CreateAudioObject(
            SATLAudioObjectData_BopAudio* const audioObjectData = nullptr)
            -> BA_GameObjectId override;
        void RemoveAudioObject(BA_UniqueId audioObjectId) override;

    protected:
        void LoadTrigger(AZ::rapidxml::xml_node<char>*);

        auto FindAudioObject(BA_UniqueId audioObjectId) -> AudioObject*;
        void PlaySound(ma_sound* soundInstance, AZ::Name const& soundName);

    private:
        SoundBank m_initSoundBank{};
        AZStd::vector<SoundBank> m_soundBanks{};

        AZStd::vector<AudioObject> m_audioObjects{};
        AZStd::unordered_map<BA_UniqueId, SoundPtr> m_soundCache{};
    };
} // namespace BopAudio
