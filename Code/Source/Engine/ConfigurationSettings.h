#pragma once

#include "AzCore/IO/Path/Path.h"

namespace BopAudio
{
    static constexpr auto BanksAlias = "@soundbanks@";
    static constexpr auto EventsAlias = "@audioevents@";
    static constexpr auto ProjectAlias = "@audioproject@";

    static constexpr auto DefaultBanksPath = "sounds/bopaudio";
    static constexpr auto DefaultProjectPath = DefaultBanksPath;
    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";
    static constexpr auto SoundbankExtension = ".soundbank";
    static constexpr auto SoundbankDependencyFileExtension{ ".soundbankdeps" };
    static constexpr auto InitBank = "init.soundbank";
    static constexpr auto InitBankSource = "init.soundbankdata";

    static constexpr AZ::IO::PathView SoundBankRefBase{ DefaultBanksPath };
    static constexpr AZ::IO::PathView SoundEventRefBase{ "sounds/bopaudio/events" };
    static constexpr AZ::IO::PathView SoundGraphRefBase{ "sounds/bopaudio/graphs" };

    auto GetBanksRootPath() -> AZ::IO::PathView;
    void SetBanksRootPath(AZ::IO::PathView banksRootPath);

    class ConfigurationSettings
    {
    public:
        auto Load(AZ::IO::PathView configFile) -> bool;
    };

} // namespace BopAudio
