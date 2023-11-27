#include "SteamAudioEditorSystemComponent.h"

#include "AzCore/PlatformIncl.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzToolsFramework/API/ViewPaneOptions.h"

#include "SteamAudio/SteamAudioTypeIds.h"
#include "SteamAudioWidget.h"
#include "Tools/AudioSystemEditor_SteamAudio.h"

namespace SteamAudio
{
    AZ_COMPONENT_IMPL( // NOLINT
        SteamAudioEditorSystemComponent,
        "SteamAudioEditorSystemComponent",
        SteamAudioEditorSystemComponentTypeId,
        BaseSystemComponent);

    void SteamAudioEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SteamAudioEditorSystemComponent, SteamAudioSystemComponent>()->Version(0);
        }
    }

    SteamAudioEditorSystemComponent::SteamAudioEditorSystemComponent() = default;

    SteamAudioEditorSystemComponent::~SteamAudioEditorSystemComponent() = default;

    void SteamAudioEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("SteamAudioEditorService"));
    }

    void SteamAudioEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("SteamAudioEditorService"));
    }

    void SteamAudioEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void SteamAudioEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("AssetDatabaseService"));
        dependent.push_back(AZ_CRC_CE("AssetCatalogService"));
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void SteamAudioEditorSystemComponent::Activate()
    {
        SteamAudioSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusConnect();
    }

    void SteamAudioEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusDisconnect();
        SteamAudioSystemComponent::Deactivate();
    }

    void SteamAudioEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 400);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/SteamAudio/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor under an Examples sub-menu
        AzToolsFramework::RegisterViewPane<SteamAudioWidget>("SteamAudio", "Examples", options);
    }

    void SteamAudioEditorSystemComponent::InitializeEditorImplPlugin()
    {
        m_editorImplPlugin = AZStd::make_unique<AudioSystemEditor_SteamAudio>();
    }

    void SteamAudioEditorSystemComponent::ReleaseEditorImplPlugin()
    {
        m_editorImplPlugin.reset();
    }

    auto SteamAudioEditorSystemComponent::GetEditorImplPlugin() -> AudioControls::IAudioSystemEditor*
    {
        return m_editorImplPlugin.get();
    }

} // namespace SteamAudio
