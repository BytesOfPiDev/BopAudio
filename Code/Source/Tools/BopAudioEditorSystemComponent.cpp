#include "BopAudioEditorSystemComponent.h"

#include "AzCore/Serialization/SerializeContext.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/BopAudioSystemComponent.h"
#include "Tools/AudioSystemEditor_BopAudio.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioEditorSystemComponent,
        "BopAudioEditorSystemComponent",
        BopAudioEditorSystemComponentTypeId,
        BaseSystemComponent);

    void BopAudioEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BopAudioEditorSystemComponent, BopAudioSystemComponent>()
                ->Version(0);
        }
    }

    BopAudioEditorSystemComponent::BopAudioEditorSystemComponent() = default;

    BopAudioEditorSystemComponent::~BopAudioEditorSystemComponent() = default;

    void BopAudioEditorSystemComponent::GetProvidedServices(
        AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("BopAudioEditorService"));
    }

    void BopAudioEditorSystemComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("BopAudioEditorService"));
    }

    void BopAudioEditorSystemComponent::GetRequiredServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void BopAudioEditorSystemComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void BopAudioEditorSystemComponent::Init()
    {
        BopAudioSystemComponent::Init();
    }

    void BopAudioEditorSystemComponent::Activate()
    {
        BopAudioSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusConnect();
    }

    void BopAudioEditorSystemComponent::Deactivate()
    {
        BopAudioSystemComponent::Deactivate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusDisconnect();
    }

    void BopAudioEditorSystemComponent::InitializeEditorImplPlugin()
    {
        m_editorImplPlugin = AZStd::make_unique<AudioSystemEditor_script>();
    }

    void BopAudioEditorSystemComponent::ReleaseEditorImplPlugin()
    {
        m_editorImplPlugin.reset();
    }

    auto BopAudioEditorSystemComponent::GetEditorImplPlugin() -> AudioControls::IAudioSystemEditor*
    {
        return m_editorImplPlugin.get();
    }

} // namespace BopAudio
