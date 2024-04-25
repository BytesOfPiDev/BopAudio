#include "Tools/AudioBopAudioLoader.h"

#include <AzCore/JSON/document.h>

#include "AudioFileUtils.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path_fwd.h"
#include "AzCore/Serialization/Utils.h"
#include "AzCore/Utils/Utils.h"

#include "IAudioSystemControl.h"
#include "IAudioSystemEditor.h"

#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Tools/AudioSystemControl_BopAudio.h"
#include "Tools/AudioSystemEditor_BopAudio.h"

namespace BopAudio
{
    void AudioBopAudioLoader::Load(AudioSystemEditor_BopAudio* audioSystemImpl)
    {
        m_audioSystemEditor = audioSystemImpl;
        AZ::IO::FixedMaxPath audioProjectFullPath{ m_audioSystemEditor->GetDataPath() };

        LoadControlsInFolder(
            AZ::IO::FixedMaxPath{ audioProjectFullPath / AudioStrings::GameParametersFolder }
                .Native());

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
        ExtractControlsFromXML(
            xmlNode,
            BopAudioControlType::Rtpc,
            AudioStrings::GameParametersFolder,
            AudioStrings::NameAttribute);
        ExtractControlsFromXML(
            xmlNode,
            BopAudioControlType::AuxBus,
            AudioStrings::AuxBusTag,
            AudioStrings::NameAttribute);

        AZStd::string_view const xmlTag(xmlNode->name());
        bool const isSwitchTag{ xmlTag == AudioStrings::SwitchGroupTag };
        bool const isStateTag{ xmlTag == AudioStrings::StateGroupTag };

        if (isSwitchTag || isStateTag)
        {
            if (auto const nameAttr = xmlNode->first_attribute(AudioStrings::NameAttribute))
            {
                AZStd::string const parentName{ nameAttr->value() };
                AudioControls::IAudioSystemControl* group =
                    m_audioSystemEditor->GetControlByName(parentName);
                if (!group)
                {
                    group = m_audioSystemEditor->CreateControl(AudioControls::SControlDef(
                        parentName,
                        isSwitchTag ? BopAudioControlType::SwitchGroup
                                    : BopAudioControlType::GameStateGroup));
                }

                auto const childrenNode = xmlNode->first_node(AudioStrings::ChildrenListTag);
                if (childrenNode)
                {
                    auto childNode = childrenNode->first_node();
                    while (childNode)
                    {
                        if (auto childNameAttr =
                                childNode->first_attribute(AudioStrings::NameAttribute))
                        {
                            m_audioSystemEditor->CreateControl(AudioControls::SControlDef(
                                childNameAttr->value(),
                                isSwitchTag ? BopAudioControlType::Switch
                                            : BopAudioControlType::GameState,
                                false,
                                group));
                        }
                        childNode = childNode->next_sibling();
                    }
                }
            }
        }

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
