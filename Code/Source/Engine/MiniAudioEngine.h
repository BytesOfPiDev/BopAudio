#pragma once

#include "AzCore/Name/Name.h"
#include "AzCore/XML/rapidxml.h"
#include "AzCore/base.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/SoundBank.h"
#include "MiniAudioIncludes.h"

namespace BopAudio
{
    class AudioSystemImpl_BopAudio;

    using SoundId = AZ::Name;

    class MiniAudioEngine : public AudioEngineInterface::Registrar
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEngine);

        MiniAudioEngine();
        ~MiniAudioEngine() override = default;

        auto Initialize() -> bool;
        auto LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool;
        auto Unload() -> bool;

        auto ActivateTrigger(BA_UniqueId baId, BA_SoundId soundId) -> bool;

        auto GetSoundEngine() -> ma_engine* override
        {
            return &m_engine;
        };

    protected:
        void LoadTrigger(AZ::rapidxml::xml_node<char>*);

    private:
        AZStd::vector<SoundBank> m_soundBanks{};

        ma_engine m_engine{};
    };
} // namespace BopAudio
