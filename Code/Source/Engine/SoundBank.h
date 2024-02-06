#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "Engine/AudioEvent.h"
#include "Engine/Id.h"
#include "Engine/Sound.h"
#include "MiniAudio/SoundAsset.h"

struct ma_sound;

namespace RAPIDJSONNAMESPACE
{
    struct Document;
}

namespace BopAudio
{
    using SoundNames = AZStd::vector<NamedResource>;

    class SoundBank
    {
        using SoundAssetMap = AZStd::unordered_map<NamedResource, MiniAudio::SoundDataAsset>;

    public:
        AZ_DEFAULT_COPY_MOVE(SoundBank);

        SoundBank() = default;
        SoundBank(AZStd::string_view soundBankFileName);
        SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData);
        virtual ~SoundBank() = default;

        static auto LoadInitBank() -> SoundBank;

        auto Load() -> bool;
        auto Load(AZStd::span<char> soundBankFileBuffer);
        [[nodiscard]] auto GetAssets() const -> SoundAssetMap const&
        {
            return m_soundAssets;
        }
        [[nodiscard]] auto GetSoundAsset(NamedResource const& soundName) const
            -> MiniAudio::SoundDataAsset;

        [[nodiscard]] auto IsEmpty() const -> bool
        {
            return !m_soundAssets.empty();
        };

    protected:
        auto LoadTriggers(rapidjson::Document const& doc) -> bool;

    private:
        NamedResource m_id{};
        AZ::Name m_soundBankName{};
        SoundAssetMap m_soundAssets{};
        [[maybe_unused]] AZStd::vector<AudioEvent> m_events{};
        Audio::SATLAudioFileEntryInfo* m_fileEntryInfo{};

    protected:
        // [[nodiscard]] auto
        // CreateSound(decltype(m_soundAssets)::const_iterator soundIter) const
        // -> SoundPtr;
    };

    /*
     * Attempts to load a sound bank into a buffer.
     *
     * @param soundBankName The O3DE project-relative path and filename of the
     * soundbank.
     * @return AZStd::vector<char> The raw file data.
     */
    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath) -> AZStd::vector<char>;

    /*
     * Parses a buffer as a sound bank file and returns the sound names within
     * it.
     *
     * @param soundBankFileBuffer A char buffer containing a sound bank's data.
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZStd::span<char> soundBankFileBuffer) -> SoundNames;

    /*
     * Loads and parses a given sound bank full file path.
     *
     * @param soundBankFilePath The O3DE project-relative file path of the sound
     * bank.
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames;

    /*
     * Loads a sound bank by it's BopAudio-project name.
     *
     * @param soundBankName The BopAudio-project name.
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames;

} // namespace BopAudio
