#pragma once

#include "AzCore/IO/Path/Path.h"
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
        [[nodiscard]] auto GetLocalizationFolder() const -> AZ::IO::PathView;

    private:
        void LoadSoundBanks(
            AZ::IO::PathView const rootFolder, AZ::IO::PathView subPath, bool isLocalized);
        void LoadControlsInFolder(AZStd::string_view const folderPath);
        void LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode);

        void ExtractControlsFromXML(
            const AZ::rapidxml::xml_node<char>* xmlNode,
            BopAudioControlType type,
            AZStd::string_view const controlTag,
            AZStd::string_view const controlNameAttribute);

    private:
        AZ::IO::Path m_localizationFolder;
        AudioSystemEditor_BopAudio* m_audioSystemImpl = nullptr;
    };

} // namespace BopAudio
