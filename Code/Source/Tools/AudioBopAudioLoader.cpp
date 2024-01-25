#include "AudioFileUtils.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path_fwd.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "IAudioSystemControl.h"
#include "IAudioSystemEditor.h"
#include "Tools/AudioSystemControl_BopAudio.h"

#include "Tools/AudioBopAudioLoader.h"
#include "Tools/AudioSystemEditor_BopAudio.h"

namespace BopAudio
{

    void AudioBopAudioLoader::Load(AudioSystemEditor_BopAudio* audioSystemImpl)
    {
        m_audioSystemImpl = audioSystemImpl;
        AZ::IO::FixedMaxPath audioProjectFullPath{ m_audioSystemImpl->GetDataPath() };

        LoadControlsInFolder(AZ::IO::FixedMaxPath{ audioProjectFullPath / BopAudioStrings::GameParametersFolder }.Native());
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ audioProjectFullPath / BopAudioStrings::EventsFolder }.Native());
        LoadSoundBanks(GetBanksRootPath(), "", false);
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
                AZ_TracePrintf("AudioBopAudioLoader", "Loading Xml from '%s'.", filePath.c_str());

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

        ExtractControlsFromXML(xmlNode, BopAudioControlType::Trigger, BopAudioStrings::EventTag, BopAudioStrings::NameAttribute);
        ExtractControlsFromXML(xmlNode, BopAudioControlType::Rtpc, BopAudioStrings::GameParametersFolder, BopAudioStrings::NameAttribute);
        ExtractControlsFromXML(xmlNode, BopAudioControlType::AuxBus, BopAudioStrings::AuxBusTag, BopAudioStrings::NameAttribute);

        AZStd::string_view const xmlTag(xmlNode->name());
        bool const isSwitchTag{ xmlTag == BopAudioStrings::SwitchGroupTag };
        bool const isStateTag{ xmlTag == BopAudioStrings::StateGroupTag };

        if (isSwitchTag || isStateTag)
        {
            if (auto const nameAttr = xmlNode->first_attribute(BopAudioStrings::NameAttribute))
            {
                AZStd::string const parentName{ nameAttr->value() };
                AudioControls::IAudioSystemControl* group = m_audioSystemImpl->GetControlByName(parentName);
                if (!group)
                {
                    group = m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                        parentName, isSwitchTag ? BopAudioControlType::SwitchGroup : BopAudioControlType::GameStateGroup));
                }

                auto const childrenNode = xmlNode->first_node(BopAudioStrings::ChildrenListTag);
                if (childrenNode)
                {
                    auto childNode = childrenNode->first_node();
                    while (childNode)
                    {
                        if (auto childNameAttr = childNode->first_attribute(BopAudioStrings::NameAttribute))
                        {
                            m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                                childNameAttr->value(),
                                isSwitchTag ? BopAudioControlType::Switch : BopAudioControlType::GameState,
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
                m_audioSystemImpl->CreateControl(AudioControls::SControlDef(nameAttr->value(), type));
            }
        }
    }

    auto AudioBopAudioLoader::GetLocalizationFolder() const -> AZStd::string const&
    {
        return m_localizationFolder;
    }

    void AudioBopAudioLoader::LoadSoundBanks(AZStd::string_view const rootFolder, AZStd::string_view const subPath, bool isLocalized)
    {
        AZ::IO::FixedMaxPath searchPath(rootFolder);
        searchPath /= subPath;

        auto const foundFiles = Audio::FindFilesInPath(searchPath.Native(), "*");
        bool isLocalizedLoaded = isLocalized;

        for (auto const& filePath : foundFiles)
        {
            AZ_Assert(
                AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()),
                "FindFiles found '%s' but FileIO says it doesn't exist!",
                filePath.c_str());

            AZ::IO::Path fileName = filePath.Filename();

            if (AZ::IO::FileIOBase::GetInstance()->IsDirectory(filePath.c_str()))
            {
                if (fileName != ExternalSourcesPath && !isLocalizedLoaded)
                {
                    // each sub-folder represents a different language,
                    // we load only one as all of them should have the
                    // same content (in the future we want to have a
                    // consistency report to highlight if this is not the case)
                    m_localizationFolder.assign(fileName.Native().data(), fileName.Native().size());
                    LoadSoundBanks(searchPath.Native(), m_localizationFolder, true);
                    isLocalizedLoaded = true;
                }
            }
            else if (fileName.Extension() == SoundbankExtension && fileName != InitBank)
            {
                m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                    AZStd::string{ fileName.Native() }, BopAudioControlType::SoundBank, isLocalized, nullptr, subPath));
            }
        }
    }

} // namespace BopAudio
