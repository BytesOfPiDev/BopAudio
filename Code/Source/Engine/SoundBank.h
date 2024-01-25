#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    class SoundBank
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SoundBank);

        SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData);
        virtual ~SoundBank() = default;

        auto Load() -> bool;

    private:
        AZStd::unordered_map<AZ::Name, MiniAudio::SoundDataAsset> m_soundAssets{};
        Audio::SATLAudioFileEntryInfo* m_fileEntryInfo{};
    };
} // namespace BopAudio
