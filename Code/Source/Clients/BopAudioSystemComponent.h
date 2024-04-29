#pragma once

#include "AzCore/Component/Component.h"

#include "BopAudio/BopAudioBus.h"
#include "Engine/AudioSystemImpl_script.h"
#include "IAudioSystem.h"

namespace BopAudio
{
    class BopAudioSystemComponent
        : public AZ::Component
        , protected BopAudioRequestBus::Handler
        , protected Audio::Gem::EngineRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(BopAudioSystemComponent);
        AZ_DISABLE_COPY_MOVE(BopAudioSystemComponent);

        BopAudioSystemComponent();
        ~BopAudioSystemComponent() override;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(
            AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        auto Initialize() -> bool override;
        void Release() override;

    private:
        AZStd::optional<AudioSystemImpl_script> m_audioSystemImpl{};
    };

} // namespace BopAudio
