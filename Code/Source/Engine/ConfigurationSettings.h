#pragma once

#include "AzCore/std/string/string_view.h"
namespace BopAudio
{
    static constexpr auto DefaultLibrariesPath = "sounds/bopaudio/";
    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";
    static constexpr auto LibraryExtension = ".soundlib";
    static constexpr auto MediaExtension = ".baf";
    static constexpr auto InitLibrary = "init.soundlib";

    auto GetLibrariesRootPath() -> AZStd::string_view const;
    void SetLibrariesRootPath(AZStd::string_view const path);

    class ConfigurationSettings
    {
    public:
        auto Load(AZStd::string_view configFile) -> bool;
    };

} // namespace BopAudio
