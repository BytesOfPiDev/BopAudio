#include "Engine/ConfigurationSettings.h"

#include "AzCore/IO/Path/Path.h"

namespace BopAudio
{
    static AZ::IO::PathView s_configuredBanksPath = BanksAlias;

    auto GetBanksRootPath() -> AZ::IO::PathView
    {
        return s_configuredBanksPath;
    }

    void SetBanksRootPath(AZ::IO::PathView banksRootPath)
    {
        s_configuredBanksPath = banksRootPath;
    }

    auto ConfigurationSettings::Load(AZ::IO::PathView /*configFile*/) -> bool
    {
        return true;
    }
} // namespace BopAudio
