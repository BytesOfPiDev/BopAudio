#pragma once

#include "AzCore/Component/Component.h"

#include "Builder/AudioControlBuilderWorker.h"

namespace BopAudio
{

    class BopAudioAssetBuilderComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT(BopAudioAssetBuilderComponent, "{A8D75486-FC8B-4F61-8F61-DB78CD424264}");
        AZ_DISABLE_COPY_MOVE(BopAudioAssetBuilderComponent);

        static void Reflect(AZ::ReflectContext* context);

        BopAudioAssetBuilderComponent();
        ~BopAudioAssetBuilderComponent() override;

        void Init() override;
        void Activate() override;
        void Deactivate() override;

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(
            AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);


    protected:
        void CreateFactories();

        void ConfigureSoundBankBuilder();
        void ConfigureAudioEventBuilder();
        void ConfigureAudioControlBuilder();

    private:
        AudioControlBuilderWorker m_audioControlBuilder;
    };
} // namespace BopAudio
