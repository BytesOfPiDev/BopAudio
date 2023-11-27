#include "AzCore/std/string/conversions.h"
#include "IAudioConnection.h"
#include "IAudioSystem.h"
#include "IAudioSystemControl.h"
#include "IAudioSystemEditor.h"
#include <AzCore/StringFunc/StringFunc.h>
#include <Tools/AudioSystemEditor_SteamAudio.h>

#include <ACETypes.h>
#include <AzCore/Utils/Utils.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <Engine/Common_SteamAudio.h>
#include <Tools/AudioSystemControl_SteamAudio.h>

namespace SteamAudio
{
    auto TagToType(const AZStd::string_view tag) -> AudioControls::TImplControlType
    {
        if (tag == XmlTags::EventTag)
        {
            return SteamAudioControlType::Trigger;
        }
        else if (tag == XmlTags::RtpcTag)
        {
            return SteamAudioControlType::Rtpc;
        }
        else if (tag == XmlTags::SwitchTag)
        {
            return SteamAudioControlType::Switch;
        }
        else if (tag == XmlTags::GameStateTag)
        {
            return SteamAudioControlType::GameState;
        }

        return SteamAudioControlType::Invalid;
    }

    auto TypeToTag([[maybe_unused]] const AudioControls::TImplControlType type) -> AZStd::string_view
    {
        switch (type)
        {
        case SteamAudioControlType::Trigger:
            return XmlTags::EventTag;
        case SteamAudioControlType::Rtpc:
            return XmlTags::RtpcTag;
        case SteamAudioControlType::Switch:
            return XmlTags::SwitchTag;
        case SteamAudioControlType::GameState:
            return XmlTags::GameStateTag;
        };

        return "Invalid";
    }

    AudioSystemEditor_SteamAudio::AudioSystemEditor_SteamAudio() = default;

    void AudioSystemEditor_SteamAudio::Reload()
    {
        AZStd::ranges::for_each(
            m_controls,
            [](auto const& idControlPair)
            {
                TControlPtr control = idControlPair.second;
                if (control)
                {
                    return;
                }

                control->SetPlaceholder(true);
            });

        m_loader.Load(this);

        m_connectionsByID.clear();
        UpdateConnectedStatus();
    }

    auto AudioSystemEditor_SteamAudio::CreateControl(const AudioControls::SControlDef& controlDefinition)
        -> AudioControls::IAudioSystemControl*
    {
        AZStd::string fullName = controlDefinition.m_name;
        AudioControls::IAudioSystemControl* parent = controlDefinition.m_parentControl;
        if (parent)
        {
            AZ::StringFunc::Path::Join(controlDefinition.m_parentControl->GetName().c_str(), fullName.c_str(), fullName);
        }

        if (!controlDefinition.m_path.empty())
        {
            AZ::StringFunc::Path::Join(controlDefinition.m_path.c_str(), fullName.c_str(), fullName);
        }

        AudioControls::CID id = GetID(fullName);

        AudioControls::IAudioSystemControl* control = GetControl(id);
        if (control)
        {
            if (control->IsPlaceholder())
            {
                control->SetPlaceholder(false);
                if (parent && parent->IsPlaceholder())
                {
                    parent->SetPlaceholder(false);
                }
            }
            return control;
        }
        else
        {
            TControlPtr newControl =
                AZStd::make_shared<IAudioSystemControl_SteamAudio>(controlDefinition.m_name, id, controlDefinition.m_type);
            if (!parent)
            {
                parent = &m_rootControl;
            }

            parent->AddChild(newControl.get());
            newControl->SetParent(parent);
            newControl->SetLocalized(controlDefinition.m_isLocalized);
            m_controls[id] = newControl;

            return newControl.get();
        }

        return {};
    }

    auto AudioSystemEditor_SteamAudio::GetControl(AudioControls::CID id) const -> AudioControls::IAudioSystemControl*
    {
        if (id != AudioControls::ACE_INVALID_CID)
        {
            auto it = m_controls.find(id);
            if (it != m_controls.end())
            {
                return it->second.get();
            }
        }

        return nullptr;
    }

    auto AudioSystemEditor_SteamAudio::ImplTypeToATLType(AudioControls::TImplControlType type) const -> AudioControls::EACEControlType
    {
        switch (type)
        {
        case SteamAudioControlType::Trigger:
            return AudioControls::eACET_TRIGGER;
        case SteamAudioControlType::Rtpc:
            return AudioControls::eACET_RTPC;
        case SteamAudioControlType::Switch:
        case SteamAudioControlType::GameState:
            return AudioControls::eACET_SWITCH;
        case SteamAudioControlType::Environment:
            return AudioControls::eACET_ENVIRONMENT;
        case SteamAudioControlType::SoundLibrary:
            return AudioControls::eACET_PRELOAD;
        }

        return AudioControls::eACET_NUM_TYPES;
    }

    auto AudioSystemEditor_SteamAudio::GetCompatibleTypes(AudioControls::EACEControlType atlControlType) const
        -> AudioControls::TImplControlTypeMask
    {
        switch (atlControlType)
        {
        case AudioControls::eACET_TRIGGER:
            return SteamAudioControlType::Trigger;
        case AudioControls::eACET_RTPC:
            return SteamAudioControlType::Rtpc;
        case AudioControls::eACET_SWITCH:
            return (SteamAudioControlType::Switch | SteamAudioControlType::GameState);
        case AudioControls::eACET_SWITCH_STATE:
            return (SteamAudioControlType::Switch | SteamAudioControlTypeNamespace::GameState | SteamAudioControlType::Rtpc);
        case AudioControls::eACET_ENVIRONMENT:
            return (SteamAudioControlType::AuxBus | SteamAudioControlType::Rtpc);
        case AudioControls::eACET_PRELOAD:
            return SteamAudioControlType::SoundLibrary;
        }
        return AudioControls::AUDIO_IMPL_INVALID_TYPE;
    }

    auto AudioSystemEditor_SteamAudio::CreateConnectionToControl(
        AudioControls::EACEControlType atlControlType, AudioControls::IAudioSystemControl* middlewareControl)
        -> AudioControls::TConnectionPtr
    {
        if (middlewareControl)
        {
            middlewareControl->SetConnected(true);
            ++m_connectionsByID[middlewareControl->GetId()];

            if (middlewareControl->GetType() == SteamAudioControlType::Rtpc)
            {
                switch (atlControlType)
                {
                case AudioControls::EACEControlType::eACET_RTPC:
                    {
                        return AZStd::make_shared<RtpcConnection>(middlewareControl->GetId());
                    }
                case AudioControls::EACEControlType::eACET_SWITCH_STATE:
                    {
                        return AZStd::make_shared<StateToRtpcConnection>(middlewareControl->GetId());
                    }
                }
            }
            return AZStd::make_shared<AudioControls::IAudioConnection>(middlewareControl->GetId());
        }
        return {};
    }

    auto AudioSystemEditor_SteamAudio::CreateConnectionFromXMLNode(
        AZ::rapidxml::xml_node<char>* node, AudioControls::EACEControlType atlControlType) -> AudioControls::TConnectionPtr
    {
        if (node)
        {
            AZStd::string_view element(node->name());
            AudioControls::TImplControlType type = TagToType(element);

            if (type != AudioControls::AUDIO_IMPL_INVALID_TYPE)
            {
                AZStd::string name{};
                AZStd::string_view localized{};

                if (auto nameAttr = node->first_attribute(SteamAudio::XmlTags::NameAttribute, 0, false); nameAttr != nullptr)
                {
                    name = nameAttr->value();
                }

                if (auto localizedAttr = node->first_attribute(XmlTags::LocalizedAttribute, 0, false))
                {
                    localized = localizedAttr->value();
                }

                bool const isLocalized = AZ::StringFunc::Equal(localized, "true");
                // If the control wasn't found, create a placeholder.
                // We want to see that connection even if it's not in the middleware.
                // User could be viewing the editor without a middleware project.

                AudioControls::IAudioSystemControl* control = GetControlByName(name, isLocalized);
                if (!control)
                {
                    control = CreateControl(AudioControls::SControlDef(name, type));
                    if (control)
                    {
                        control->SetPlaceholder(true);
                        control->SetLocalized(isLocalized);
                    }
                }

                // If it's a switch we connect to one of the states within the switch
                if (type == SteamAudioControlType::SwitchGroup || type == SteamAudioControlType::GameStateGroup)
                {
                    if (auto childNode = node->first_node(); childNode != nullptr)
                    {
                        AZStd::string childName{};
                        if (auto childNameAttr = childNode->first_attribute(XmlTags::NameAttribute, 0, false); childNameAttr != nullptr)
                        {
                            childName = childNameAttr->value();
                        }

                        AudioControls::IAudioSystemControl* childControl = GetControlByName(childName, false, control);
                        if (!childControl)
                        {
                            childControl = CreateControl(AudioControls::SControlDef(
                                childName,
                                type == SteamAudioControlType::SwitchGroup ? SteamAudioControlType::Switch
                                                                           : SteamAudioControlType::GameState,
                                false,
                                control));
                        }
                        control = childControl;
                    }
                }
                if (control)
                {
                    control->SetConnected(true);
                    m_connectionsByID[control->GetId()];

                    if (type == SteamAudioControlType::Rtpc)
                    {
                        switch (atlControlType)
                        {
                        case AudioControls::EACEControlType::eACET_RTPC:
                            {
                                RtpcConnectionPtr connection{ AZStd::make_shared<RtpcConnection>(control->GetId()) };
                                float mult = 1.0f;
                                float shift = 0.0f;

                                if (auto multAttr = node->first_attribute(XmlTags::MultiplierAttribute, 0, false); multAttr != nullptr)
                                {
                                    mult = AZStd::stof(AZStd::string(multAttr->value()));
                                    if (auto shiftAttr = node->first_attribute(XmlTags::ShiftAttribute, 0, false); shiftAttr != nullptr)
                                    {
                                        shift = AZStd::stof(AZStd::string(shiftAttr->value()));
                                    }

                                    connection->m_mult = mult;
                                    connection->m_shift = shift;
                                    return connection;
                                }
                            }

                        case AudioControls::EACEControlType::eACET_SWITCH_STATE:
                            {
                                StateConnectionPtr connection = AZStd::make_shared<StateToRtpcConnection>(control->GetId());

                                float value{};
                                if (auto valueAttr = node->first_attribute(XmlTags::ValueAttribute, 0, false); valueAttr != nullptr)
                                {
                                    value = AZStd::stof(AZStd::string(valueAttr->value()));
                                }

                                connection->m_value = value;
                                return connection;
                            }
                        case AudioControls::EACEControlType::eACET_ENVIRONMENT:
                            {
                                return AZStd::make_shared<AudioControls::IAudioConnection>(control->GetId());
                            }
                        }
                    }
                    else
                    {
                        return AZStd::make_shared<AudioControls::IAudioConnection>(control->GetId());
                    }
                }
            }
        }
        return {};
    }

    auto AudioSystemEditor_SteamAudio::CreateXMLNodeFromConnection(
        const AudioControls::TConnectionPtr connection, const AudioControls::EACEControlType atlControlType)
        -> AZ::rapidxml::xml_node<char>*
    {
        return {};
    }

    auto AudioSystemEditor_SteamAudio::GetTypeIcon(AudioControls::TImplControlType type) const -> const AZStd::string_view
    {
        switch (type)
        {
        case SteamAudioControlType::Trigger:
            return ":/SteamAudio/trigger_icon.svg";
        default:
            return {};
        };
    }

    auto AudioSystemEditor_SteamAudio::GetTypeIconSelected(AudioControls::TImplControlType type) const -> const AZStd::string_view
    {
        return {};
    }

    auto AudioSystemEditor_SteamAudio::GetName() const -> AZStd::string
    {
        return "O3DE-SteamAudio";
    }
    auto AudioSystemEditor_SteamAudio::GetDataPath() const -> AZ::IO::FixedMaxPath
    {
        auto projectPath = AZ::IO::FixedMaxPath{ AZ::Utils::GetProjectPath() };
        return (projectPath / "sounds" / "steamaudio_project");
    }

    void AudioSystemEditor_SteamAudio::ConnectionRemoved(AudioControls::IAudioSystemControl* control)
    {
    }

    auto AudioSystemEditor_SteamAudio::GetControlByName(
        AZStd::string name, bool isLocalized, AudioControls::IAudioSystemControl* parent) const -> AudioControls::IAudioSystemControl*
    {
        if (parent)
        {
            AZ::StringFunc::Path::Join(parent->GetName().c_str(), name.c_str(), name);
        }

        if (isLocalized)
        {
            AZ::StringFunc::Path::Join(m_loader.GetLocalizationFolder().c_str(), name.c_str(), name);
        }

        return GetControl(GetID(name));
    }

    auto AudioSystemEditor_SteamAudio::GetID(const AZStd::string_view name) const -> AudioControls::CID
    {
        return Audio::AudioStringToID<AudioControls::CID>(name.data());
    }

    void AudioSystemEditor_SteamAudio::UpdateConnectedStatus()
    {
    }
} // namespace SteamAudio
