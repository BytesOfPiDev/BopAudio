#pragma once

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Component/Component.h"
#include "AzCore/Component/TickBus.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "SteamAudio/SteamAudioBus.h"
#include "phonon.h"

namespace SteamAudio
{
    class SteamAudioSystemComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
        , protected SteamAudioRequestBus::Handler
        , protected Audio::Gem::EngineRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(SteamAudioSystemComponent); // NOLINT
        AZ_DISABLE_COPY_MOVE(SteamAudioSystemComponent); // NOLINT

        SteamAudioSystemComponent();
        ~SteamAudioSystemComponent() override;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // SteamAudioRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override; ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Audio::Gem::EngineRequestBus interface implementation
        auto Initialize() -> bool override;
        void Release() override;
        ////////////////////////////////////////////////////////////////////////

        [[nodiscard]] auto GetEngine() const -> Audio::AudioSystemImplementation const*
        {
            return m_engineSteamAudio.get();
        }

        auto ModifyEngine() -> Audio::AudioSystemImplementation*
        {
            return m_engineSteamAudio.get();
        }

    private:
        IPLContextSettings m_contextSettings{};
        IPLContext m_context{ nullptr };
        IPLHRTFSettings m_hrtfSettings{};
        IPLAudioSettings m_audioSettings{};
        IPLHRTF m_hrtf = nullptr;

        AZStd::unique_ptr<Audio::AudioSystemImplementation> m_engineSteamAudio;
        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers{};
    };

} // namespace SteamAudio
