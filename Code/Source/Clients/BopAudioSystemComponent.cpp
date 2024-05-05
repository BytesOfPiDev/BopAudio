#include "BopAudioSystemComponent.h"

#include "AzCore/Console/IConsole.h"
#include "AzCore/Console/IConsoleTypes.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "IAudioSystem.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Engine/ConfigurationSettings.h"
#include "ScriptCanvas/Nodes/AudioControlNode.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId);

    void BopAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* const serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BopAudioSystemComponent, AZ::Component>()->Version(0);
        }
    }

    void BopAudioSystemComponent::GetProvidedServices(
        AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioService"));
        provided.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void BopAudioSystemComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioService"));
        incompatible.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void BopAudioSystemComponent::GetRequiredServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("AssetDatabaseService"));
        required.push_back(AZ_CRC_CE("AssetCatalogService"));
    }

    void BopAudioSystemComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("AudioSystemService"));
    }

    BopAudioSystemComponent::BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == nullptr)
        {
            BopAudioInterface::Register(this);
        }

        Audio::Gem::EngineRequestBus::Handler::BusConnect();
        m_audioSystemImpl.emplace();
    }

    BopAudioSystemComponent::~BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == this)
        {
            BopAudioInterface::Unregister(this);
        }

        Audio::Gem::EngineRequestBus::Handler::BusDisconnect();
        m_audioSystemImpl.reset();
    }

    void BopAudioSystemComponent::Init()
    {
        RegisterFileAliases();

        // HACK: Nodeable registration is confusing.  Just hacking it until the O3DE documentation
        // makes it clear how to reliably register a node. Ensuring compiler doesn't remove the type
        // for being unused.
        {
            auto node{ AZStd::make_unique<Nodes::AudioControlNode>() };
            if (node->RTTI_IsTypeOf(AZ::TypeId{}))
            {
                AZ_Info("BopAudioSystemComponent", "BlahBlahBlah");
            }
        }
    }

    void BopAudioSystemComponent::Activate()
    {
        BopAudioRequestBus::Handler::BusConnect();
    }

    void BopAudioSystemComponent::Deactivate()
    {
        BopAudioRequestBus::Handler::BusDisconnect();
    }

    auto BopAudioSystemComponent::Initialize() -> bool
    {
        Audio::SystemRequest::Initialize initRequest;
        AZ::Interface<Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initRequest));

        return true;
    }

    void BopAudioSystemComponent::Release()
    {
        m_audioSystemImpl.reset();
    }

    void BopAudioSystemComponent::RegisterFileAliases()
    {
        auto* const fileIo = AZ::IO::FileIOBase::GetInstance();

        AZ_Error(
            "BopAudioSystemComponent",
            fileIo,
            "Failed to get the FileIO instance. Aliases will not be set.\n");

        if (fileIo)
        {
            fileIo->SetAlias(ProjectAlias, "Sounds/bopaudio/");
            fileIo->SetAlias(EventsAlias, "Sounds/bopaudio/events/");
        }
    }
} // namespace BopAudio
