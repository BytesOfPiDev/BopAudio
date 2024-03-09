#pragma once

#include "AzCore/IO/Path/Path.h"

namespace BopAudio
{
    static constexpr auto BanksAlias = "@soundbanks@";
    static constexpr auto EventsAlias = "@audioevents@";
    static constexpr auto ProjectAlias = "@audioproject@";

    static constexpr auto DefaultBanksPath = "sounds/bopaudio";
    static constexpr auto SoundSourcePath = "sounds/bopaudio/data";
    static constexpr auto DefaultProjectPath = DefaultBanksPath;
    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";
    static constexpr auto SoundbankExtension = ".soundbank";
    static constexpr auto SoundbankDependencyFileExtension{ ".soundbankdeps" };
    static constexpr auto InitBank = "init.soundbank";
    static constexpr auto InitBankSource = "init.soundbankdata";

    static constexpr AZ::IO::PathView EventsPath{ "sounds/bopaudio/events" };

    auto GetBanksRootPath() -> AZ::IO::PathView;
    void SetBanksRootPath(AZ::IO::PathView banksRootPath);

    static constexpr auto DefaultAudioChannels = 2;
    static constexpr auto DefaultSampleRate = 48000;
    static constexpr auto DefaultBitsPerSample = 16;

    class ConfigurationSettings
    {
    public:
        auto Load(AZ::IO::PathView configFile) -> bool;
    };

} // namespace BopAudio
