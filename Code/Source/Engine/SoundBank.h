#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "Engine/AudioEvent.h"
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

        static auto LoadInitBank() -> SoundBank;

        auto Load() -> bool;
        auto Load(AZStd::span<char> soundBankFileBuffer);
        auto GetSoundAsset(AZ::Name const& soundName) -> MiniAudio::SoundDataAsset
        {
            if (!m_soundAssets.contains(soundName))
            {
                return {};
            }

            return m_soundAssets[soundName];
        };
        auto CreateSound(AZ::Name const& soundName) -> SoundPtr;

        [[nodiscard]] auto IsEmpty() const -> bool
        {
            return !m_soundAssets.empty();
        };

    private:
        AZStd::unordered_map<AZ::Name, MiniAudio::SoundDataAsset> m_soundAssets{};
        [[maybe_unused]] AZStd::vector<AudioEvent> m_events{};
        Audio::SATLAudioFileEntryInfo* m_fileEntryInfo{};
    };

    /*
     * Attempts to load a sound bank into a buffer.
     *
     * @param soundBankName The O3DE project-relative path and filename of the soundbank.
     * @return AZStd::vector<char> The raw file data.
     */
    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath) -> AZStd::vector<char>;

    /*
     * Parses a buffer as a sound bank file and returns the sound names within it.
     *
     * @param soundBankFileBuffer A char buffer containing a sound bank's data.
     * @return SoundNames The container of sound names found within the sound bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZStd::span<char> soundBankFileBuffer) -> SoundNames;

    /*
     * Loads and parses a given sound bank full file path.
     *
     * @param soundBankFilePath The O3DE project-relative file path of the sound bank.
     * @return SoundNames The container of sound names found within the sound bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames;

    /*
     * Loads a sound bank by it's BopAudio-project name.
     *
     * @param soundBankName The BopAudio-project name.
     * @return SoundNames The container of sound names found within the sound bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames;

} // namespace BopAudio
