#pragma once

#include "AzCore/IO/Path/Path_fwd.h"

namespace BopAudio
{
    static constexpr auto DefaultBanksPath = "sounds/bopaudio/";
    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";
    static constexpr auto SoundbankExtension = ".soundlib";
    static constexpr auto SoundbankDependencyFileExtension{ ".soundbankdeps" };
    static constexpr auto MediaExtension = ".baf";
    static constexpr auto InitBank = "init.soundlib";

    auto GetBanksRootPath() -> AZ::IO::PathView;
    void SetBanksRootPath(AZ::IO::PathView banksRootPath);

    class ConfigurationSettings
    {
    public:
        auto Load(AZ::IO::PathView configFile) -> bool;
    };

} // namespace BopAudio
