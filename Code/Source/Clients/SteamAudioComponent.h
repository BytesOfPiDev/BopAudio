#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/base.h"

#include "SteamAudio/AudioAsset.h"
#include "SteamAudio/SteamAudioComponentBus.h"
#include "SteamAudio/SteamAudioTypeIds.h"

namespace SteamAudio
{
    class SteamAudioComponent
        : public AZ::Component
        , protected SteamAudioComponentRequestBus::Handler
    {
    public:
        AZ_COMPONENT(SteamAudioComponent, SteamAudioComponentTypeId);
        AZ_DISABLE_COPY_MOVE(SteamAudioComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        SteamAudioComponent() = default;
        ~SteamAudioComponent() override = default;

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    private:
        AZ::Data::Asset<AudioAsset> m_audioAsset{};
    };
} // namespace SteamAudio
