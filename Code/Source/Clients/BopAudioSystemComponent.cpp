#include "BopAudioSystemComponent.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/PlatformId/PlatformDefaults.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Settings/SettingsRegistry.h"
#include "AzCore/Utils/Utils.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAssetHandler.h"
#include "Clients/SoundBankAsset.h"
#include "Clients/SoundBankAssetHandler.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngine.h"
#include "IAudioSystem.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId);

    void BopAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        SoundBankAsset::Reflect(context);

        if (auto* const serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<MiniAudioEventRequests>()->Version(0);

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
    }

    void BopAudioSystemComponent::RegisterFileAliases()
    {
        AZ::IO::Path const banksPath = []() -> decltype(banksPath)
        {
            auto path{ decltype(banksPath){ AZ::Utils::GetProjectProductPathForPlatform() } };
            return path / "sounds/bopaudio/banks";
        }();

        AZ::IO::Path const eventsPath = []() -> decltype(eventsPath)
        {
            auto path{ decltype(banksPath){ AZ::Utils::GetProjectProductPathForPlatform() } };
            return path / "sounds/bopaudio/events";
        }();

        AZ::IO::Path const projectPath = []() -> decltype(projectPath)
        {
            auto path{ decltype(banksPath){ AZ::Utils::GetProjectProductPathForPlatform() } };
            return path / "sounds/bopaudio";
        }();

        AZ::IO::FileIOBase::GetInstance()->SetAlias(BanksAlias, banksPath.c_str());
        AZ::IO::FileIOBase::GetInstance()->SetAlias(EventsAlias, eventsPath.c_str());
        AZ::IO::FileIOBase::GetInstance()->SetAlias(ProjectAlias, projectPath.c_str());
    }

    void BopAudioSystemComponent::Activate()
    {
        Audio::Gem::EngineRequestBus::Handler::BusConnect();
        BopAudioRequestBus::Handler::BusConnect();
    }

    void BopAudioSystemComponent::Deactivate()
    {
        Audio::Gem::EngineRequestBus::Handler::BusDisconnect();
        BopAudioRequestBus::Handler::BusDisconnect();
    }

    auto BopAudioSystemComponent::Initialize() -> bool
    {
        AZ::SettingsRegistryInterface::FixedValueString assetPlatform =
            AZ::OSPlatformToDefaultAssetPlatform(AZ_TRAIT_OS_PLATFORM_CODENAME);

        m_miniAudioEngine = AZStd::make_unique<MiniAudioEngine>();

        if (m_audioSystemImpl =
                AZStd::make_unique<BopAudio::AudioSystemImpl_miniaudio>(assetPlatform.c_str());
            m_audioSystemImpl)
        {
            Audio::SystemRequest::Initialize initRequest;
            AZ::Interface<Audio::IAudioSystem>::Get()->PushRequestBlocking(
                AZStd::move(initRequest));

            AZLOG_INFO("AudioEngineBopAudio created!");
            return true;
        }
        else
        {
            AZLOG_ERROR("Could not create AudioEngineBopAudio!");
        }
        return false;
    }

    void BopAudioSystemComponent::Release()
    {
        m_audioSystemImpl = nullptr;
        m_miniAudioEngine = nullptr;
    }

} // namespace BopAudio
