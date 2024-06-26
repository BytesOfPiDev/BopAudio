#pragma once

#include "AzCore/IO/Path/Path.h"

namespace BopAudio
{
    static constexpr auto BanksAlias{ "@scriptsoundbanks" };
    static constexpr auto EventsAlias = "@scriptaudioevents@";
    static constexpr auto ProjectAlias = "@scriptaudioproject@";

    static constexpr auto ExternalSourcesPath = "external";
    static constexpr auto ConfigFile = "bopaudio_config.json";

    static constexpr auto SoundbankDependencyFileExtension{ ".nopeavi" };

    auto GetBanksRootPath() -> AZ::IO::PathView;
    void SetBanksRootPath(AZ::IO::PathView banksRootPath);

    static constexpr auto DefaultAudioChannels = 2;
    static constexpr auto DefaultSampleRate = 48000;
    static constexpr auto DefaultBitsPerSample = 16;

    AZ_ENUM_CLASS(Language, en, es);

    class ConfigurationSettings
    {
    public:
        auto Load(AZ::IO::PathView configFile) -> bool;

        [[nodiscard]] auto GetLanguage() const -> Language
        {
            return m_language;
        }

    private:
        Language m_language{};
    };

} // namespace BopAudio
