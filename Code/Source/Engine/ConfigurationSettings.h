#pragma once

#include "AzCore/std/string/string_view.h"
namespace BopAudio
{
    static constexpr auto DefaultBanksPath = "sounds/bopaudio/";
    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";
    static constexpr auto SoundbankExtension = ".soundlib";
    static constexpr auto SoundbankDependencyFileExtension{ ".soundbankdeps" };
    static constexpr auto MediaExtension = ".baf";
    static constexpr auto InitBank = "init.soundlib";

    auto GetBanksRootPath() -> AZStd::string_view const;
    void SetLibrariesRootPath(AZStd::string_view const path);

    class ConfigurationSettings
    {
    public:
        auto Load(AZStd::string_view configFile) -> bool;
    };

} // namespace BopAudio
