#pragma once

#include "ACETypes.h"
#include "AzCore/XML/rapidxml.h"

#include "Tools/AudioSystemControl_SteamAudio.h"

namespace SteamAudio
{
    class AudioSystemEditor_SteamAudio;

    class AudioSteamAudioLoader
    {
    public:
        AudioSteamAudioLoader() = default;

        void Load(AudioSystemEditor_SteamAudio* audioSystemImpl);
        [[nodiscard]] auto GetLocalizationFolder() const -> AZStd::string const&;

    private:
        void LoadSoundBanks(AZStd::string_view const rootFolder, AZStd::string_view const subPath, bool isLocalized);
        void LoadControlsInFolder(AZStd::string_view const folderPath);
        void LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode);

        void ExtractControlsFromXML(
            const AZ::rapidxml::xml_node<char>* xmlNode,
            SteamAudioControlType type,
            const AZStd::string_view controlTag,
            const AZStd::string_view controlNameAttribute);

    private:
        AZStd::string m_localizationFolder;
        AudioSystemEditor_SteamAudio* m_audioSystemImpl = nullptr;
    };

} // namespace SteamAudio
