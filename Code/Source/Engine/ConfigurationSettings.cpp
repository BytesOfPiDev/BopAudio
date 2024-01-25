#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{

    static AZStd::string_view s_configuredBanksPath = DefaultBanksPath; // NOLINT

    auto GetLibrariesRootPath() -> AZStd::string_view const
    {
        return s_configuredBanksPath;
    }

    void SetLibrariesRootPath(AZStd::string_view const path)
    {
        s_configuredBanksPath = path;
    }

    auto ConfigurationSettings::Load(AZStd::string_view configFile) -> bool
    {
        return true;
    }
} // namespace BopAudio
