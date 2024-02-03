#pragma once

#include <ACETypes.h>
#include <IAudioConnection.h>
#include <IAudioSystemControl.h>
#include <IAudioSystemEditor.h>

#include "Tools/AudioBopAudioLoader.h"

namespace BopAudio
{
    class RtpcConnection : public AudioControls::IAudioConnection
    {
    public:
        AZ_DEFAULT_COPY_MOVE(RtpcConnection); // NOLINT

        explicit RtpcConnection(AudioControls::CID id)
            : IAudioConnection(id)
        {
        }

        ~RtpcConnection() override = default;

        auto HasProperties() -> bool override
        {
            return true;
        }

        float m_mult = 1.0f;
        float m_shift = 0.0f;
    };

    using RtpcConnectionPtr = AZStd::shared_ptr<RtpcConnection>;

    class StateToRtpcConnection : public AudioControls::IAudioConnection
    {
    public:
        explicit StateToRtpcConnection(AudioControls::CID id)
            : IAudioConnection(id)
        {
        }

        auto HasProperties() -> bool override
        {
            return true;
        }

        float m_value = 0.0f;
    };

    using StateConnectionPtr = AZStd::shared_ptr<StateToRtpcConnection>;

    class AudioSystemEditor_BopAudio : public AudioControls::IAudioSystemEditor // NOLINT
    {
    public:
        AudioSystemEditor_BopAudio();
        ~AudioSystemEditor_BopAudio() override = default;

        //////////////////////////////////////////////////////////
        // IAudioSystemEditor implementation
        /////////////////////////////////////////////////////////
        void Reload() override;
        auto CreateControl(AudioControls::SControlDef const& controlDefinition) -> AudioControls::IAudioSystemControl* override;
        auto GetRoot() -> AudioControls::IAudioSystemControl* override
        {
            return &m_rootControl;
        }
        [[nodiscard]] auto GetControl(AudioControls::CID id) const -> AudioControls::IAudioSystemControl* override;
        [[nodiscard]] auto ImplTypeToATLType(AudioControls::TImplControlType type) const -> AudioControls::EACEControlType override;
        [[nodiscard]] auto GetCompatibleTypes(AudioControls::EACEControlType atlControlType) const
            -> AudioControls::TImplControlTypeMask override;
        auto CreateConnectionToControl(AudioControls::EACEControlType atlControlType, AudioControls::IAudioSystemControl* middlewareControl)
            -> AudioControls::TConnectionPtr override;
        auto CreateConnectionFromXMLNode(AZ::rapidxml::xml_node<char>* node, AudioControls::EACEControlType atlControlType)
            -> AudioControls::TConnectionPtr override;
        auto CreateXMLNodeFromConnection(
            AudioControls::TConnectionPtr const connection, AudioControls::EACEControlType const atlControlType)
            -> AZ::rapidxml::xml_node<char>* override;
        [[nodiscard]] auto GetTypeIcon(AudioControls::TImplControlType type) const -> AZStd::string_view const override;
        [[nodiscard]] auto GetTypeIconSelected(AudioControls::TImplControlType type) const -> AZStd::string_view const override;
        [[nodiscard]] auto GetName() const -> AZStd::string override;
        [[nodiscard]] auto GetDataPath() const -> AZ::IO::FixedMaxPath override;
        void DataSaved() override
        {
        }
        void ConnectionRemoved(AudioControls::IAudioSystemControl* control) override;
        //////////////////////////////////////////////////////////

        auto GetControlByName(AZStd::string name, bool isLocalized = false, AudioControls::IAudioSystemControl* parent = nullptr) const
            -> AudioControls::IAudioSystemControl*;

    private:
        // Gets the ID of the control given its name. As controls can have the same name
        // if they're under different parents, the name of the parent is also needed (if there is one)
        [[nodiscard]] auto GetID(AZStd::string_view const name) const -> AudioControls::CID;

        void UpdateConnectedStatus();

        AudioControls::IAudioSystemControl m_rootControl;

        using TControlPtr = AZStd::shared_ptr<AudioControls::IAudioSystemControl>;
        using TControlMap = AZStd::unordered_map<AudioControls::CID, TControlPtr>;
        TControlMap m_controls;

        using TConnectionsMap = AZStd::unordered_map<AudioControls::CID, int>;
        TConnectionsMap m_connectionsByID;
        AudioBopAudioLoader m_loader;
    };
} // namespace BopAudio
