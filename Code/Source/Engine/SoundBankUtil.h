#pragma once

#include <AzCore/JSON/document.h>

#include "AzCore/Name/Name.h"
#include "BopAudio/Util.h"
#include "Engine/Id.h"

struct ma_sound;

namespace BopAudio
{
    using SoundNames = AZStd::vector<AZ::Name>;

    /*
     * Attempts to load a sound bank into a buffer.
     *
     * @param soundBankName The absolute path of the soundbank.
     *
     * @return AZStd::vector<char> The raw file data.
     */
    [[nodiscard]] auto LoadSoundBankToBuffer(AZ::IO::PathView soundBankFilePath)
        -> AudioOutcome<AZStd::vector<char>>;

    [[nodiscard]] auto LoadEventIds(rapidjson::Document const& doc)
        -> AudioOutcome<AZStd::vector<AudioEventId>>;

    auto GetSoundNamesFromSoundBankFile(rapidjson::Document const& doc) -> SoundNames;
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
     * @param soundBankFilePath The absolute path of the sound bank.
     * @return SoundNames The container of sound names found within the sound
     * bank.
     */
    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames;

} // namespace BopAudio
