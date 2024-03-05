#pragma once

#include "AzCore/base.h"
#include "IAudioInterfacesCommonData.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/Sound.h"
#include "Engine/SoundSource.h"

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
        [[nodiscard]] auto RegisterFile(RegisterFileData const&) -> NullOutcome override;
        [[nodiscard]] auto Shutdown() -> bool override;

        [[nodiscard]] auto GetSoundEngine() -> ma_engine* override;

        [[nodiscard]] auto ActivateTrigger(ActivateEventData const& activateTriggerRequest)
            -> NullOutcome override;

        [[nodiscard]] auto CreateAudioObject() -> AudioObjectId override;
        void RemoveAudioObject(AudioObjectId targetAudioObjectId) override;

    protected:
        void LoadSounds();
        void LoadEvents();

        [[nodiscard]] auto FindAudioEvent(AudioEventId targetAudioEvent)
            -> AZ::Data::Asset<AudioEventAsset>;
        [[nodiscard]] auto FindAudioObject(AudioObjectId targetAudioObjectId) -> AudioObject*;
        auto LoadSound(ResourceRef const& resourceRef) -> NullOutcome override;

    private:
        static constexpr auto MaxAudioObjects{ 4096 };
        AudioObject m_globalObject{};
        AZStd::fixed_vector<AudioObject, MaxAudioObjects> m_audioObjects{};

        AZStd::map<Audio::TAudioControlID, AZ::Data::Asset<AudioEventAsset>> m_controlEventMap{};
        AZStd::unordered_set<Audio::TAudioSourceId> m_loadedSources{};
        AZStd::unordered_map<AudioEventId, AZ::Data::Asset<AudioEventAsset>> m_eventAssets{};
        AZStd::unordered_map<ResourceRef, AZStd::unique_ptr<SoundSource>> m_soundSourceMap{};
    };
} // namespace BopAudio
