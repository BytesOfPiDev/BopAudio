#pragma once

#include "AzCore/Component/Component.h"

#include "BopAudio/BopAudioBus.h"

namespace BopAudio
{
    class BopAudioSystemComponent
        : public AZ::Component
        , protected BopAudioRequestBus::Handler
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

    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };

} // namespace BopAudio
