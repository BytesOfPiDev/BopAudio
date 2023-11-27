#include "Engine/Common_SteamAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "IAudioSystemControl.h"
#include "IAudioSystemEditor.h"
#include "Tools/AudioSystemControl_SteamAudio.h"
#include <AzCore/IO/FileIO.h>
#include <Tools/AudioSystemEditor_SteamAudio.h>

#include <AudioFileUtils.h>
#include <AzCore/IO/Path/Path_fwd.h>
#include <IAudioSystem.h>
#include <Tools/AudioSteamAudioLoader.h>

namespace SteamAudio
{
    namespace SteamAudioStrings
    {
        // Project Folders
        static constexpr auto GameParametersFolder = "GameParameters";
        static constexpr auto EventsFolder = "Events";

        // Xml Tags
        // static constexpr auto GameParameterTag = "GameParameter";
        static constexpr auto EventTag = "Event";
        static constexpr auto NameAttribute = "Name";
        static constexpr auto AuxBusTag = "AuxBus";
        static constexpr auto SwitchGroupTag = "SwitchGroup";
        static constexpr auto StateGroupTag = "StateGroup";
        static constexpr auto ChildrenListTag = "ChildrenList";

    } // namespace SteamAudioStrings

    void AudioSteamAudioLoader::Load(AudioSystemEditor_SteamAudio* audioSystemImpl)
    {
        m_audioSystemImpl = audioSystemImpl;
        AZ::IO::FixedMaxPath steamAudioProjectFullPath{ m_audioSystemImpl->GetDataPath() };

        LoadControlsInFolder(AZ::IO::FixedMaxPath{ steamAudioProjectFullPath / SteamAudioStrings::GameParametersFolder }.Native());
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ steamAudioProjectFullPath / SteamAudioStrings::EventsFolder }.Native());
        LoadSoundBanks(GetLibrariesRootPath(), "", false);
    }

    void AudioSteamAudioLoader::LoadControlsInFolder(const AZStd::string_view folderPath)
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
                AZ_TracePrintf("AudioSteamAudioLoader", "Loading Xml from '%s'.", filePath.c_str());

                Audio::ScopedXmlLoader xmlFileLoader(filePath.Native());
                if (!xmlFileLoader.HasError())
                {
                    LoadControl(xmlFileLoader.GetRootNode());
                }
            }
        }
    }

    void AudioSteamAudioLoader::LoadControl(AZ::rapidxml::xml_node<char> const* xmlNode)
    {
        if (!xmlNode)
        {
            return;
        }

        ExtractControlsFromXML(xmlNode, SteamAudioControlType::Trigger, SteamAudioStrings::EventTag, SteamAudioStrings::NameAttribute);
        ExtractControlsFromXML(
            xmlNode, SteamAudioControlType::Rtpc, SteamAudioStrings::GameParametersFolder, SteamAudioStrings::NameAttribute);
        ExtractControlsFromXML(xmlNode, SteamAudioControlType::AuxBus, SteamAudioStrings::AuxBusTag, SteamAudioStrings::NameAttribute);

        AZStd::string_view xmlTag(xmlNode->name());
        bool isSwitchTag{ xmlTag == SteamAudioStrings::SwitchGroupTag };
        bool isStateTag{ xmlTag == SteamAudioStrings::StateGroupTag };

        if (isSwitchTag || isStateTag)
        {
            if (auto nameAttr = xmlNode->first_attribute(SteamAudioStrings::NameAttribute))
            {
                AZStd::string const parentName{ nameAttr->value() };
                AudioControls::IAudioSystemControl* group = m_audioSystemImpl->GetControlByName(parentName);
                if (!group)
                {
                    group = m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                        parentName, isSwitchTag ? SteamAudioControlType::SwitchGroup : SteamAudioControlType::GameStateGroup));
                }

                auto childrenNode = xmlNode->first_node(SteamAudioStrings::ChildrenListTag);
                if (childrenNode)
                {
                    auto childNode = childrenNode->first_node();
                    while (childNode)
                    {
                        if (auto childNameAttr = childNode->first_attribute(SteamAudioStrings::NameAttribute))
                        {
                            m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                                childNameAttr->value(),
                                isSwitchTag ? SteamAudioControlType::Switch : SteamAudioControlType::GameState,
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

    void AudioSteamAudioLoader::ExtractControlsFromXML(
        AZ::rapidxml::xml_node<char> const* xmlNode,
        SteamAudioControlType type,
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

    auto AudioSteamAudioLoader::GetLocalizationFolder() const -> AZStd::string const&
    {
        return m_localizationFolder;
    }

    void AudioSteamAudioLoader::LoadSoundBanks(AZStd::string_view const rootFolder, AZStd::string_view const subPath, bool isLocalized)
    {
        AZ::IO::FixedMaxPath searchPath(rootFolder);
        searchPath /= subPath;

        auto foundFiles = Audio::FindFilesInPath(searchPath.Native(), "*");
        bool isLocalizedLoaded = isLocalized;

        AZStd::ranges::for_each(
            foundFiles,
            [&isLocalizedLoaded, this, &searchPath, &isLocalized, &subPath](auto const& filePath) -> void
            {
                AZ_Assert(
                    AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()),
                    "FindFiles found '%s' but FileIO says it doesn't exist!",
                    filePath.c_str());

                AZ::IO::PathView fileName = filePath.Filename();

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
                else if (fileName.Extension() == LibraryExtension && fileName != InitLibrary)
                {
                    m_audioSystemImpl->CreateControl(AudioControls::SControlDef(
                        AZStd::string{ fileName.Native() }, SteamAudioControlType::SoundLibrary, isLocalized, nullptr, subPath));
                }
            });
    }

} // namespace SteamAudio
