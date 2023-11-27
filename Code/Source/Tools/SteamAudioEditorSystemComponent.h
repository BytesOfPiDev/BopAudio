
#pragma once

#include "Clients/SteamAudioSystemComponent.h"

#include "AzToolsFramework/Entity/EditorEntityContextBus.h"
#include "IAudioSystemEditor.h"

#include "Tools/AudioSystemEditor_SteamAudio.h"

namespace SteamAudio
{
    /// System component for SteamAudio editor
    class SteamAudioEditorSystemComponent
        : public SteamAudioSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
        , protected AudioControlsEditor::EditorImplPluginEventBus::Handler
    {
        using BaseSystemComponent = SteamAudioSystemComponent;

    public:
        AZ_COMPONENT_DECL(SteamAudioEditorSystemComponent); // NOLINT
        AZ_DISABLE_COPY_MOVE(SteamAudioEditorSystemComponent); // NOLINT

        static void Reflect(AZ::ReflectContext* context);

        SteamAudioEditorSystemComponent();
        ~SteamAudioEditorSystemComponent() override;

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    protected:
        // AzToolsFramework::EditorEventsBus overrides ...
        void NotifyRegisterViews() override;

        ////////////////////////////////////////////////////////////////////////
        // AudioControlsEditor::EditorImplPluginEventBus interface implementation
        void InitializeEditorImplPlugin() override;
        void ReleaseEditorImplPlugin() override;
        auto GetEditorImplPlugin() -> AudioControls::IAudioSystemEditor* override;
        ////////////////////////////////////////////////////////////////////////

    private:
        AZStd::unique_ptr<AudioControls::IAudioSystemEditor> m_editorImplPlugin;
    };
} // namespace SteamAudio
