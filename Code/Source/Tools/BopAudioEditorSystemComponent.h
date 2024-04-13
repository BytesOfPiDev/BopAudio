#pragma once

#include "Clients/BopAudioSystemComponent.h"

#include "AzToolsFramework/API/ToolsApplicationAPI.h"
#include "IAudioSystemEditor.h"
#include "Tools/EditorAudioEventAsset.h"

namespace BopAudio
{
    /// System component for BopAudio editor
    class BopAudioEditorSystemComponent
        : public BopAudioSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
        , protected AudioControlsEditor::EditorImplPluginEventBus::Handler
    {
        using BaseSystemComponent = BopAudioSystemComponent;

    public:
        AZ_COMPONENT_DECL(BopAudioEditorSystemComponent);
        AZ_DISABLE_COPY_MOVE(BopAudioEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        BopAudioEditorSystemComponent();
        ~BopAudioEditorSystemComponent() override;

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(
            AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        // AzToolsFramework::EditorEventsBus overrides ...
        void NotifyRegisterViews() override;

        ////////////////////////////////////////////////////////////////////////
        // AudioControlsEditor::EditorImplPluginEventBus interface
        // implementation
        void InitializeEditorImplPlugin() override;
        void ReleaseEditorImplPlugin() override;
        auto GetEditorImplPlugin() -> AudioControls::IAudioSystemEditor* override;
        ////////////////////////////////////////////////////////////////////////

    private:
        AZStd::unique_ptr<AudioControls::IAudioSystemEditor> m_editorImplPlugin;
        AZStd::unique_ptr<EditorAudioEventAssetHandler> m_editorAudioEventAssetHandler;
    };
} // namespace BopAudio
