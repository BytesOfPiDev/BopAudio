#pragma once

#include "AzCore/base.h"
#include "IAudioInterfacesCommonData.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"

namespace BopAudio
{
    class MiniAudioEngine final : public SoundEngine::Registrar
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEngine);
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(MiniAudioEngine);

        MiniAudioEngine() = default;
        ~MiniAudioEngine() override;

        [[nodiscard]] auto Initialize() -> AudioOutcome<void> override;
        [[nodiscard]] auto RegisterFile(RegisterFileData const&) -> NullOutcome override;
        [[nodiscard]] auto Shutdown() -> NullOutcome override;

        [[nodiscard]] auto GetSoundEngine() -> ma_engine* override;

        [[nodiscard]] auto StartEvent(StartEventData const& activateTriggerRequest)
            -> NullOutcome override;

        [[nodiscard]] auto CreateAudioObject() -> AudioObjectId override;
        void RemoveAudioObject(AudioObjectId targetAudioObjectId) override;

        [[nodiscard]] auto IsInit() const -> bool override
        {
            return m_isInit;
        }

    protected:
        void LoadSounds();
        void LoadEvents();

        auto StopEvent(AudioEventId eventId) -> bool override;

        [[nodiscard]] auto FindAudioObject(AudioObjectId targetAudioObjectId) -> AudioObject*;
        auto LoadSound(SoundRef const& resourceRef) -> NullOutcome override;

    private:
        static constexpr auto MaxAudioObjects{ 4096 };
        AudioObject m_globalObject{};
        AZStd::fixed_vector<AudioObject, MaxAudioObjects> m_audioObjects{};

        AZStd::map<Audio::TAudioControlID, AZ::Data::Asset<AudioEventAsset>> m_controlEventMap{};
        AZStd::unordered_set<Audio::TAudioSourceId> m_loadedSources{};

        AZStd::vector<AZ::Data::Asset<AudioEventAsset>> m_eventAssets{};
        bool m_isInit{};
    };
} // namespace BopAudio
