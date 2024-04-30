#pragma once

#include "AzCore/IO/Path/Path.h"
#include "AzCore/XML/rapidxml.h"

#include "Tools/AudioSystemControl_BopAudio.h"

namespace BopAudio
{
    class AudioSystemEditor_script;

    class AudioBopAudioLoader
    {
    public:
        AudioBopAudioLoader() = default;

        void Load(AudioSystemEditor_script* audioSystemImpl);
        [[nodiscard]] auto GetLocalizationFolder() const -> AZ::IO::PathView;

    private:
        void LoadControlsInFolder(AZStd::string_view const folderPath);
        void LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode);

        void ExtractControlsFromXML(
            const AZ::rapidxml::xml_node<char>* xmlNode,
            BopAudioControlType type,
            AZStd::string_view const controlTag,
            AZStd::string_view const controlNameAttribute);

    private:
        AZ::IO::Path m_localizationFolder;
        AudioSystemEditor_script* m_audioSystemEditor = nullptr;
    };

} // namespace BopAudio
