#include "BopAudioSystemComponent.h"

#include "AzCore/Console/IConsole.h"
#include "AzCore/Console/IConsoleTypes.h"
#include "AzCore/Serialization/SerializeContext.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "IAudioSystem.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(
        BopAudioSystemComponent, "BopAudioSystemComponent", BopAudioSystemComponentTypeId);

    AZ_CVAR(
        bool,
        cl_enable_script_audiosystemimplementation,
        true,
        nullptr,
        AZ::ConsoleFunctorFlags::ReadOnly,
        "Enable the Script AudioSystemImplementation. Takes effect only during startup.");

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
    }

    void BopAudioSystemComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioService"));
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

        if (cl_enable_script_audiosystemimplementation)
        {
            Audio::Gem::EngineRequestBus::Handler::BusConnect();
        }
        else
        {
            AZ_Warning(
                "BopAudioSystemComponent",
                false,
                "Script AudioSystemImplementation will not due to CVAR.");
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
    }

    void BopAudioSystemComponent::Deactivate()
    {
//        m_audioSystemImpl.reset();

        BopAudioRequestBus::Handler::BusDisconnect();
    }

    auto BopAudioSystemComponent::Initialize() -> bool
    {
        if (m_audioSystemImpl.has_value())
        {
            AZ_Error("BopAudioSystemComponent", false, "AudioSystemImpl already initialized.");

            return false;
        }

        m_audioSystemImpl.emplace();

        Audio::SystemRequest::Initialize initRequest;
        AZ::Interface<Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initRequest));

        return true;
    }

    void BopAudioSystemComponent::Release()
    {
    }
} // namespace BopAudio
