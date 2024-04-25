#include "BopAudioSystemComponent.h"

#include "AzCore/IO/FileIO.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"
#include "Clients/AudioEventAsset.h"
#include "Engine/Id.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAssetHandler.h"
#include "Engine/ConfigurationSettings.h"
#include "ScriptCanvas/Nodes/AudioControlNode.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId);

    void BopAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        AudioEventId::Reflect(context);
        AudioObjectId::Reflect(context);
        StartEventData::Reflect(context);
        StopEventData::Reflect(context);

        AudioEventAsset::Reflect(context);

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
        required.push_back(AZ_CRC_CE("AudioSystemService"));
    }

    void BopAudioSystemComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("MiniAudioService"));
    }

    BopAudioSystemComponent::BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == nullptr)
        {
            BopAudioInterface::Register(this);
        }
    }

    BopAudioSystemComponent::~BopAudioSystemComponent()
    {
        if (BopAudioInterface::Get() == this)
        {
            BopAudioInterface::Unregister(this);
        }
    }

    void BopAudioSystemComponent::Init()
    {
        AZStd::make_unique<Nodes::AudioControlNode>();
    }

    void BopAudioSystemComponent::RegisterFileAliases()
    {
        auto const banksPath = []()
        {
            static constexpr auto path{ "@products@/sounds/bopaudio/banks" };
            return AZ::IO::FileIOBase::GetInstance()->ResolvePath(path).value_or("");
        }();

        auto const eventsPath = []()
        {
            static constexpr auto path{ "@products@/sounds/bopaudio/events" };
            return AZ::IO::FileIOBase::GetInstance()->ResolvePath(path).value_or("");
        }();

        auto const projectPath = []()
        {
            static constexpr auto path{ "@products@/sounds/bopaudio" };
            return AZ::IO::FileIOBase::GetInstance()->ResolvePath(path).value_or("");
        }();

        AZ::IO::FileIOBase::GetInstance()->SetAlias(BanksAlias, banksPath.c_str());
        AZ::IO::FileIOBase::GetInstance()->SetAlias(EventsAlias, eventsPath.c_str());
        AZ::IO::FileIOBase::GetInstance()->SetAlias(ProjectAlias, projectPath.c_str());
    }

    void BopAudioSystemComponent::Activate()
    {
        RegisterFileAliases();

        m_audioEventAssetHandler.Register();

        BopAudioRequestBus::Handler::BusConnect();
    }

    void BopAudioSystemComponent::Deactivate()
    {
        BopAudioRequestBus::Handler::BusDisconnect();

        m_audioEventAssetHandler.Unregister();
    }

} // namespace BopAudio
