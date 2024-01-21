#pragma once

#include "AzCore/std/string/string_view.h"
namespace BopAudio
{
    static constexpr const auto DefaultLibrariesPath = "sounds/steamaudio/";
    static constexpr const auto ExternalSourcesPath = "external";
    static constexpr const auto ConfigFile = "steamaudio_config.json";
    static constexpr const auto LibraryExtension = ".salib";
    static constexpr const auto MediaExtension = ".saf";
    static constexpr const auto InitLibrary = "init.salib";

    auto GetLibrariesRootPath() -> AZStd::string_view const;
    void SetLibrariesRootPath(AZStd::string_view const path);

    class ConfigurationSettings
    {
    public:
        auto Load(AZStd::string_view configFile) -> bool;
    };

} // namespace BopAudio
