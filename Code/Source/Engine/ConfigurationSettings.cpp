#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{

    static AZStd::string_view s_configuredBanksPath = DefaultLibrariesPath; // NOLINT

    auto GetLibrariesRootPath() -> const AZStd::string_view
    {
        return s_configuredBanksPath;
    }

    void SetLibrariesRootPath(const AZStd::string_view path)
    {
        s_configuredBanksPath = path;
    }

    auto ConfigurationSettings::Load(AZStd::string_view configFile) -> bool
    {
        return false;
    }
} // namespace BopAudio
