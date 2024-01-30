#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "Engine/Sound.h"
#include "MiniAudio/SoundAsset.h"

struct ma_sound;

namespace BopAudio
{
    using SoundName = AZ::Name;
    using SoundNames = AZStd::vector<SoundName>;

    class SoundBank
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SoundBank);

        SoundBank() = default;
        SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData);
        virtual ~SoundBank() = default;

        auto Load() -> bool;
        auto GetSoundAsset(AZ::Name const& soundName) -> MiniAudio::SoundDataAsset
        {
            if (!m_soundAssets.contains(soundName))
            {
                return {};
            }

            return m_soundAssets[soundName];
        };
        auto CreateSound(AZ::Name const& soundName) -> SoundPtr;

    private:
        AZStd::unordered_map<AZ::Name, MiniAudio::SoundDataAsset> m_soundAssets{};
        Audio::SATLAudioFileEntryInfo* m_fileEntryInfo{};
    };

    auto GetSoundNamesFromSoundBankFile(AZ::IO::Path soundBankFilePath) -> SoundNames;
} // namespace BopAudio
