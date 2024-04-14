#include "Scene/Groups/AudioEventGroup.h"

#include "AzCore/Serialization/EditContext.h"

namespace BopAudioScene
{
    char const* const AudioEventGroup::s_disabledOption = "No audio event";

    AudioEventGroup::AudioEventGroup() = default;

    AudioEventGroup::~AudioEventGroup() = default;

    void AudioEventGroup::Reflect(AZ::ReflectContext* context)
    {
        // There are different kind of contexts, but for groups and rules, the only one that's
        // interesting is the SerializeContext. Check if the provided context is one.
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }

        // Next, specify the fields that need to be serialized to and from a manifest. This allows
        // new fields to be stored and loaded from the manifest (.assetinfo file). These are also
        // needed for the edit context below.
        serializeContext->Class<AudioEventGroup, AZ::SceneAPI::DataTypes::IManifestObject>()
            ->Version(1)
            ->Field("groupName", &AudioEventGroup::m_groupName)
            ->Field("graphLogRoot", &AudioEventGroup::m_graphLogRoot)
            ->Field("logProcessingEvents", &AudioEventGroup::m_logProcessingEvents);

        // The EditContext allows you to add additional meta information to the previously
        // registered fields. This meta information will be used in the Scene Settings, which uses
        // the Reflected Property Editor.
        AZ::EditContext* editContext = serializeContext->GetEditContext();
        if (editContext)
        {
            editContext->Class<AudioEventGroup>("Logger", "Add additional logging to the SceneAPI.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute("AutoExpand", true)
                ->Attribute(AZ::Edit::Attributes::NameLabelOverride, "")
                ->DataElement(
                    AZ::Edit::UIHandlers::Default,
                    &AudioEventGroup::m_groupName,
                    "Name",
                    "The name of the group will be used in the log")
                // The Reflected Property Editor can pick a default editor for many types. However,
                // for the string that will store the selected node, a more specialized editor is
                // needed. NodeListSelection is one such editor and it is SceneGraph-aware. It
                // allows the selection of a specific node from the graph and the selectable items
                // can be filtered. You can find other available editors in the "RowWidgets"-folder
                // of the SceneUI.
                ->DataElement(
                    AZ_CRC("NodeListSelection", 0x45c54909),
                    &AudioEventGroup::m_graphLogRoot,
                    "Graph log root",
                    "Select the node in the graph to list children of to the log, or disable "
                    "logging.")
                ->Attribute(AZ_CRC("DisabledOption", 0x6cd17278), s_disabledOption)
                // Nodes in the SceneGraph can be marked as endpoints. To the graph, this means that
                // these nodes are not allowed to have children. While not a true one-to-one
                // mapping, endpoints often act as attributes to a node. For example, a transform
                // can be marked as an endpoint. This means that it applies its transform to the
                // parent object like an attribute. If the transform is not marked as an endpoint,
                // then it is the root transform for the group(s) that are its children.
                ->Attribute(AZ_CRC("ExcludeEndPoints", 0x53bd29cc), true)
                ->DataElement(
                    AZ::Edit::UIHandlers::Default,
                    &AudioEventGroup::m_logProcessingEvents,
                    "Log processing events",
                    "Log processing events as they happen.");
        }
    }

    void AudioEventGroup::SetName(AZStd::string const& name)
    {
        m_groupName = name;
    }

    void AudioEventGroup::SetName(AZStd::string&& name)
    {
        m_groupName = AZStd::move(name);
    }

    AZStd::string const& AudioEventGroup::GetName() const
    {
        return m_groupName;
    }

    // Groups need to provide a unique id that will be used to create the final sub id for the
    // product build using this group. While new groups created through the UI can remain fully
    // random, it's important that ids used for defaults are recreated the same way every time. It's
    // recommended this is done by using the source guid of the file and calling
    // DataTypes::Utilities::CreateStableUuid. If the id doesn't remain stable between updates this
    // will cause the sub id to change which will in turn cause the objects links to those products
    // to break.
    //
    // As this example doesn't have a product, the id is not important so just always return the
    // randomly generated id.
    const AZ::Uuid& AudioEventGroup::GetId() const
    {
        return m_id;
    }

    // Groups have the minimal amount of options to generate a working product in the cache and
    // nothing more. A group might not be perfect or contain all the data the user would expect, but
    // it will load in the engine and not crash. You can add additional settings to fine tune the
    // exporting process in the form of rules (or "modifiers" in the Scene Settings UI). Rules
    // usually group a subset of settings together, such as control of physics or level of detail.
    // This approach keeps UI clutter to a minimum by only presenting options that are relevant for
    // the user's file, while still providing access to all available settings.
    //
    // By using the "GetAvailableModifiers" in the ManifestMetaInfoHandler EBus, it's possible to
    // filter out any options that are not relevant to the group. For example, if a group only
    // allows for a single instance of a rule, it would no longer be added to this call if there is
    // already one. Because the logging doesn't require any rules, empty defaults are provided.
    auto AudioEventGroup::GetRuleContainer() -> AZ::SceneAPI::Containers::RuleContainer&
    {
        return m_ruleContainer;
    }

    auto AudioEventGroup::GetRuleContainerConst() const
        -> const AZ::SceneAPI::Containers::RuleContainer&
    {
        return m_ruleContainer;
    }

    auto AudioEventGroup::GetGraphLogRoot() const -> AZStd::string const&
    {
        return m_graphLogRoot;
    }

    auto AudioEventGroup::DoesLogGraph() const -> bool
    {
        return m_graphLogRoot.compare(s_disabledOption) != 0;
    }

    auto AudioEventGroup::DoesLogProcessingEvents() const -> bool
    {
        return m_logProcessingEvents;
    }

    void AudioEventGroup::ShouldLogProcessingEvents(bool state)
    {
        m_logProcessingEvents = state;
    }

} // namespace BopAudioScene
