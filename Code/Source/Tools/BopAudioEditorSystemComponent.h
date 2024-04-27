#pragma once

#include "Clients/BopAudioSystemComponent.h"

#include "AzToolsFramework/API/ToolsApplicationAPI.h"
#include "IAudioSystemEditor.h"

namespace BopAudio
{
    /// System component for BopAudio editor
    class BopAudioEditorSystemComponent
        : public BopAudioSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
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

    private:
        AZStd::unique_ptr<AudioControls::IAudioSystemEditor> m_editorImplPlugin;
    };
} // namespace BopAudio
