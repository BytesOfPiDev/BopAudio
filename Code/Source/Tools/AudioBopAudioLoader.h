#pragma once

#include "AzCore/XML/rapidxml.h"

#include "Tools/AudioSystemControl_BopAudio.h"

namespace BopAudio
{
    class AudioSystemEditor_BopAudio;

    class AudioBopAudioLoader
    {
    public:
        AudioBopAudioLoader() = default;

        void Load(AudioSystemEditor_BopAudio* audioSystemImpl);
        [[nodiscard]] auto GetLocalizationFolder() const -> AZStd::string const&;

    private:
        void LoadSoundBanks(AZStd::string_view const rootFolder, AZStd::string_view const subPath, bool isLocalized);
        void LoadControlsInFolder(AZStd::string_view const folderPath);
        void LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode);

        void ExtractControlsFromXML(
            const AZ::rapidxml::xml_node<char>* xmlNode,
            BopAudioControlType type,
            const AZStd::string_view controlTag,
            const AZStd::string_view controlNameAttribute);

    private:
        AZStd::string m_localizationFolder;
        AudioSystemEditor_BopAudio* m_audioSystemImpl = nullptr;
    };

} // namespace BopAudio
