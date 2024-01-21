#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/base.h"

#include "BopAudio/AudioAsset.h"
#include "BopAudio/BopAudioComponentBus.h"
#include "BopAudio/BopAudioTypeIds.h"

namespace BopAudio
{
    class BopAudioComponent
        : public AZ::Component
        , protected BopAudioComponentRequestBus::Handler
    {
    public:
        AZ_COMPONENT(BopAudioComponent, BopAudioComponentTypeId);
        AZ_DISABLE_COPY_MOVE(BopAudioComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        BopAudioComponent() = default;
        ~BopAudioComponent() override = default;

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    private:
        AZ::Data::Asset<AudioAsset> m_audioAsset{};
    };
} // namespace BopAudio
