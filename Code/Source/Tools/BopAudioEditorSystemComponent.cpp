#include "BopAudioEditorSystemComponent.h"

#include "AzCore/Serialization/SerializeContext.h"
#include "AzToolsFramework/API/ViewPaneOptions.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "BopAudioWidget.h"
#include "Tools/AudioSystemEditor_BopAudio.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioEditorSystemComponent, "BopAudioEditorSystemComponent", BopAudioEditorSystemComponentTypeId, BaseSystemComponent);

    void BopAudioEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BopAudioEditorSystemComponent, BopAudioSystemComponent>()->Version(0);
        }
    }

    BopAudioEditorSystemComponent::BopAudioEditorSystemComponent() = default;

    BopAudioEditorSystemComponent::~BopAudioEditorSystemComponent() = default;

    void BopAudioEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("BopAudioEditorService"));
    }

    void BopAudioEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("BopAudioEditorService"));
    }

    void BopAudioEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void BopAudioEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("AssetDatabaseService"));
        dependent.push_back(AZ_CRC_CE("AssetCatalogService"));
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void BopAudioEditorSystemComponent::Activate()
    {
        BopAudioSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusConnect();
    }

    void BopAudioEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusDisconnect();
        BopAudioSystemComponent::Deactivate();
    }

    void BopAudioEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 400);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/BopAudio/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor under an Examples sub-menu
        AzToolsFramework::RegisterViewPane<BopAudioWidget>("BopAudio", "Examples", options);
    }

    void BopAudioEditorSystemComponent::InitializeEditorImplPlugin()
    {
        m_editorImplPlugin = AZStd::make_unique<AudioSystemEditor_BopAudio>();
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
