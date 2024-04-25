#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"
#include "Clients/AudioEventAssetHandler.h"
#include "Clients/SoundBankAssetHandler.h"
#include "IAudioSystem.h"

#include "BopAudio/BopAudioBus.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/MiniAudioEngine.h"

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

        static void RegisterFileAliases();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Audio::Gem::EngineRequestBus interface implementation
        auto Initialize() -> bool override;
        void Release() override;
        ////////////////////////////////////////////////////////////////////////

    private:
        AZStd::unique_ptr<BopAudio::AudioSystemImpl_miniaudio> m_audioSystemImpl;
        AZStd::unique_ptr<MiniAudioEngine> m_miniAudioEngine;

        AudioEventAssetHandler m_audioEventAssetHandler;
        SoundBankAssetHandler m_soundBankAssetHandler;
    };

} // namespace BopAudio
