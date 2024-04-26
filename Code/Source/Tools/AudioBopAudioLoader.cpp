#include "Tools/AudioBopAudioLoader.h"

#include <AzCore/JSON/document.h>

#include "AudioFileUtils.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path_fwd.h"
#include "AzCore/Serialization/Utils.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Engine/Common_BopAudio.h"
#include "IAudioSystemEditor.h"

#include "Engine/ConfigurationSettings.h"
#include "Tools/AudioSystemControl_BopAudio.h"
#include "Tools/AudioSystemEditor_BopAudio.h"

namespace BopAudio
{
    void AudioBopAudioLoader::Load(AudioSystemEditor_BopAudio* /*audioSystemImpl*/)
    {
        LoadControlsInFolder(EventsAlias);
    }

    void AudioBopAudioLoader::LoadControlsInFolder(AZStd::string_view const folderPath)
    {
        auto foundFiles = Audio::FindFilesInPath(folderPath, "*");

        for (auto const& filePath : foundFiles)
        {
            AZ_Assert( // NOLINT
                AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()),
                "FindFiles found file '%s' but FileIO says it doesn't exist!",
                filePath.c_str());

            if (AZ::IO::FileIOBase::GetInstance()->IsDirectory(filePath.c_str()))
            {
                LoadControlsInFolder(filePath.Native());
            }
            else
            {
                if (AZ::StringFunc::Equal(
                        filePath.Extension().Native(), AudioEventAsset::ProductExtensionPattern))
                {
                    AZ_Info(
                        "AudioBopAudioLoader",
                        "Loading AudioEventAsset from '%s'",
                        filePath.c_str());

                    AZStd::unique_ptr<AudioEventAsset> audioEventAsset{
                        AZ::Utils::LoadObjectFromFile<AudioEventAsset>(filePath.c_str())
                    };

                    if (!audioEventAsset)
                    {
                        AZ_Error(
                            "AudioBopAudioLoader",
                            false,
                            "Failed to load audio event at path '%s'",
                            filePath.c_str());

                        continue;
                    }

                    LoadControl(*audioEventAsset, filePath.Filename().Stem().String());
                    continue;
                }

                AZ_Trace("AudioBopAudioLoader", "Loading Xml from '%s'.", filePath.c_str());

                Audio::ScopedXmlLoader xmlFileLoader(filePath.Native());
                if (!xmlFileLoader.HasError())
                {
                    LoadControl(xmlFileLoader.GetRootNode());
                }
            }
        }
    }

    void AudioBopAudioLoader::LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode)
    {
        if (!xmlNode)
        {
            return;
        }

        ExtractControlsFromXML(
            xmlNode,
            BopAudioControlType::Event,
            AudioStrings::EventTag,
            AudioStrings::NameAttribute);

        auto childNode = xmlNode->first_node();
        while (childNode)
        {
            LoadControl(childNode);
            childNode = childNode->next_sibling();
        }
    }

    void AudioBopAudioLoader::LoadControl(AudioEventAsset const&, AZStd::string_view eventName)
    {
        m_audioSystemEditor->CreateControl(
            AudioControls::SControlDef(eventName, BopAudioControlType::Event));
    }

    void AudioBopAudioLoader::ExtractControlsFromXML(
        AZ::rapidxml::xml_node<char> const* xmlNode,
        BopAudioControlType type,
        AZStd::string_view const controlTag, // NOLINT
        AZStd::string_view const controlNameAttribute) // NOLINT
    {
        AZStd::string_view xmlTag(xmlNode->name());
        if (xmlTag == controlTag)
        {
            if (auto nameAttr = xmlNode->first_attribute(controlNameAttribute.data()))
            {
                m_audioSystemEditor->CreateControl(
                    AudioControls::SControlDef(nameAttr->value(), type));
            }
        }
    }

    auto AudioBopAudioLoader::GetLocalizationFolder() const -> AZ::IO::PathView
    {
        return m_localizationFolder;
    }
} // namespace BopAudio
