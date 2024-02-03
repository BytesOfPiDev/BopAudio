#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/Component/TickBus.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"
#include "Engine/MiniAudioEngine.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/BopAudioBus.h"

namespace BopAudio
{
    class BopAudioSystemComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
        , protected BopAudioRequestBus::Handler
        , protected Audio::Gem::EngineRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(BopAudioSystemComponent); // NOLINT
        AZ_DISABLE_COPY_MOVE(BopAudioSystemComponent); // NOLINT

        BopAudioSystemComponent();
        ~BopAudioSystemComponent() override;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(
            AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // BopAudioRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate()
            override; ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Audio::Gem::EngineRequestBus interface implementation
        auto Initialize() -> bool override;
        void Release() override;
        ////////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr auto GetEngine() const -> Audio::AudioSystemImplementation const*
        {
            return m_audioSystemImpl.get();
        }

    private:
        AZStd::unique_ptr<Audio::AudioSystemImplementation> m_audioSystemImpl;
        AZStd::unique_ptr<MiniAudioEngine> m_miniAudioEngine;
    };

} // namespace BopAudio
