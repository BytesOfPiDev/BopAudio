#pragma once

#include "AzCore/Component/Component.h"

#include "Builder/BopAudioControlBuilderWorker.h"

namespace BopAudio
{

    class BopAudioControlBuilderComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT_DECL(BopAudioControlBuilderComponent);

        static void Reflect(AZ::ReflectContext* context);

        BopAudioControlBuilderComponent();
        ~BopAudioControlBuilderComponent() override;

        void Init() override;
        void Activate() override;
        void Deactivate() override;

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    private:
        BopAudioControlBuilderWorker m_audioControlBuilder;
    };
} // namespace BopAudio
