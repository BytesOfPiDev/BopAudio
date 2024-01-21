#include "BopAudioSystemComponent.h"

#include "AzCore/Console/ILogger.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Settings/SettingsRegistry.h"
#include "AzFramework/Platform/PlatformDefaults.h"

#include "Clients/AudioAssetHandler.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "BopAudio/AudioAsset.h"
#include "BopAudio/BopAudioTypeIds.h"
#include "phonon.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId); // NOLINT

    void BopAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        AudioAsset::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<BopAudioSystemComponent, AZ::Component>()->Version(0);
        }
    }

    void BopAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioService"));
        provided.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void BopAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioService"));
        incompatible.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void BopAudioSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("AudioSystemService"));
    }

    void BopAudioSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC("AudioSystemService"));
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

    void BopAudioSystemComponent::Activate()
    {
        BopAudioRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        Audio::Gem::EngineRequestBus::Handler::BusConnect();

        /*
              AZ::Data::AssetCatalogRequestBus::Broadcast(
                  &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        */
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);

        m_assetHandlers.emplace_back(aznew BopAudioAssetHandler()); // NOLINT
    }

    void BopAudioSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        Audio::Gem::EngineRequestBus::Handler::BusDisconnect();
        BopAudioRequestBus::Handler::BusDisconnect();
    }

    void BopAudioSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

    auto BopAudioSystemComponent::Initialize() -> bool
    {
        AZ::SettingsRegistryInterface::FixedValueString assetPlatform =
            AzFramework::OSPlatformToDefaultAssetPlatform(AZ_TRAIT_OS_PLATFORM_CODENAME);

        m_engineBopAudio = AZStd::make_unique<BopAudio::AudioSystemImpl_BopAudio>(assetPlatform.c_str());
        if (m_engineBopAudio)
        {
            Audio::SystemRequest::Initialize initRequest;
            AZ::Interface<Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initRequest));

            return true;
            AZLOG_INFO("AudioEngineBopAudio created!"); // NOLINT
        }
        else
        {
            AZLOG_ERROR("Could not create AudioEngineBopAudio!"); // NOLINT
        }
        return false;
    }

    void BopAudioSystemComponent::Release()
    {
        m_engineBopAudio = nullptr;
    }

} // namespace BopAudio
