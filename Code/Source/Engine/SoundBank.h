#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "BopAudio/Util.h"
#include "Engine/AudioEvent.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"
#include "MiniAudio/SoundAsset.h"

struct ma_sound;

namespace RAPIDJSONNAMESPACE
{
    struct Document;
}

namespace BopAudio
{
    using SoundNames = AZStd::vector<AZ::Name>;

    /*
     * Holdings a collection of assets used by the sound engine.
     *
     * This class will hold assets and data that need to be loaded at the same
     * time, such as on a particularly level.
     */
    class SoundBank
    {
        using SoundAssetMap = AZStd::unordered_map<ResourceRef, SoundSource>;

    public:
        AZ_DEFAULT_COPY_MOVE(SoundBank);

        SoundBank() = default;
        SoundBank(AZStd::string_view soundBankFileName);
        SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData);
        virtual ~SoundBank() = default;

        [[nodiscard]] constexpr auto operator==(ResourceRef const& resourceId) const
        {
            return m_id == resourceId;
        }

        [[nodiscard]] auto GetResourceId() const -> ResourceRef
        {
            return m_id;
        }

        [[nodiscard]] static auto LoadInitBank() -> AZ::Outcome<SoundBank, char const*>;

        auto Load() -> AZ::Outcome<void, char const*>;
        [[nodiscard]] auto GetAssets() const -> SoundAssetMap const&
        {
            return m_soundAssets;
        }
        [[nodiscard]] auto GetSoundAsset(ResourceRef const& soundName) const
            -> MiniAudio::SoundDataAsset;

        [[nodiscard]] auto GetDocument() const -> AZStd::shared_ptr<rapidjson::Document>
        {
            return m_doc;
        }

        [[nodiscard]] auto IsEmpty() const -> bool
        {
            return !m_soundAssets.empty();
        };

        [[nodiscard]] auto CloneEvent(AudioEventId eventId) const -> AudioOutcome<AudioEvent>;
        [[nodiscard]] auto CloneEvent(ResourceRef const& resourceId) const
            -> AudioOutcome<AudioEvent>;

    protected:
        auto LoadBuffer() -> bool;
        auto LoadSoundNames() -> bool;
        auto LoadSounds() -> bool;
        auto LoadEvents() -> AudioOutcome<void>;

    private:
        ResourceRef m_id{};
        SoundNames m_soundNames{};

        AZStd::optional<AZStd::vector<char>> m_optionalBuffer{};
        AZStd::span<char const> m_bufferRef{};
        AZStd::shared_ptr<rapidjson::Document> m_doc{};
        AZ::Name m_soundBankName{};
        SoundAssetMap m_soundAssets{};
        AZStd::vector<AudioEvent> m_events{};
        AudioEventIdContainer m_eventIds{};
        Audio::SATLAudioFileEntryInfo* m_fileEntryInfo{};
    };

    /*
     * Attempts to load a sound bank into a buffer.
     *
     * @param soundBankName The O3DE project-relative path and filename of the
     * soundbank.
     *
     * @return AZStd::vector<char> The raw file data.
     */
    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath)
        -> AZ::Outcome<AZStd::vector<char>, char const*>;

    [[nodiscard]] auto LoadEventIds(rapidjson::Document const& doc)
        -> AudioOutcome<AZStd::vector<AudioEventId>>;

    /*
     * Parses a buffer as a sound bank file and returns the sound names within
     * it.
     *
     * @param soundBankFileBuffer A char buffer containing a sound bank's data.
     *
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZStd::span<char const> soundBankFileBuffer) -> SoundNames;

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
     *
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames;

} // namespace BopAudio
