#include "Builder/BopAudioAssetBuilderComponent.h"

#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/RTTI/RTTIMacros.h"

#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "Builder/AudioEventAssetBuilderWorker.h"
#include "Builder/SoundBankAssetBuilderWorker.h"
#include "Clients/BopAudioSystemComponent.h"

namespace BopAudio
{

    void BopAudioAssetBuilderComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BopAudioAssetBuilderComponent, AZ::Component>()->Version(0)->Attribute(
                AZ::Edit::Attributes::SystemComponentTags,
                AZStd::vector<AZ::Crc32>({ AssetBuilderSDK::ComponentTags::AssetBuilder }));

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<BopAudioAssetBuilderComponent>(
                    "BopAudioAssetBuilderComponent", "");
            }
        }
    }

    BopAudioAssetBuilderComponent::BopAudioAssetBuilderComponent() = default;
    BopAudioAssetBuilderComponent::~BopAudioAssetBuilderComponent() = default;

    void BopAudioAssetBuilderComponent::Init()
    {
    }

    void BopAudioAssetBuilderComponent::Activate()
    {
        BopAudioSystemComponent::RegisterFileAliases();

        ConfigureAudioControlBuilder();
        ConfigureAudioEventBuilder();
        ConfigureSoundBankBuilder();
    }

    void BopAudioAssetBuilderComponent::Deactivate()
    {
        m_bankBuilderWorker.BusDisconnect();
        m_audioControlBuilder.BusDisconnect();
        m_eventBuilder.BusDisconnect();
    }

    void BopAudioAssetBuilderComponent::GetProvidedServices(
        AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioAssetBuilderService"));
        provided.push_back(AZ_CRC_CE("BopAudioEventAssetBuilderService"));
        provided.push_back(AZ_CRC_CE("BopAudioSoundBankBuilderService"));
    }

    void BopAudioAssetBuilderComponent::GetIncompatibleServices(
        AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioAssetBuilderService"));
        incompatible.push_back(AZ_CRC_CE("BopAudioEventAssetBuilderService"));
        incompatible.push_back(AZ_CRC_CE("BopAudioSoundBankBuilderService"));
    }

    void BopAudioAssetBuilderComponent::GetRequiredServices(
        AZ::ComponentDescriptor::DependencyArrayType& /*required*/){};

    void BopAudioAssetBuilderComponent::GetDependentServices(
        AZ::ComponentDescriptor::DependencyArrayType& /*dependent*/){};

    void BopAudioAssetBuilderComponent::ConfigureSoundBankBuilder()
    {
        AssetBuilderSDK::AssetBuilderDesc builderDescriptor{};
        builderDescriptor.m_name = "BopAudio SoundBank Builder";

        builderDescriptor.m_patterns.push_back(AssetBuilderSDK::AssetBuilderPattern(
            R"((.*sounds\/bopaudio\/banks\/).*\.soundbankdata)",
            AssetBuilderSDK::AssetBuilderPattern::PatternType::Regex));

        builderDescriptor.m_busId = azrtti_typeid<SoundBankAssetBuilderWorker>();
        builderDescriptor.m_version = 0;
        builderDescriptor.m_createJobFunction =
            [ObjectPtr = &m_bankBuilderWorker](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->CreateJobs(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };
        builderDescriptor.m_processJobFunction =
            [ObjectPtr = &m_bankBuilderWorker](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->ProcessJob(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };

        m_bankBuilderWorker.BusConnect(builderDescriptor.m_busId);

        AssetBuilderSDK::AssetBuilderBus::Broadcast(
            &AssetBuilderSDK::AssetBuilderBus::Events::RegisterBuilderInformation,
            builderDescriptor);
    }

    void BopAudioAssetBuilderComponent::ConfigureAudioEventBuilder()
    {
        AssetBuilderSDK::AssetBuilderDesc builderDescriptor{};
        builderDescriptor.m_name = "BopAudio AudioEvent Builder";

        builderDescriptor.m_patterns.push_back(AssetBuilderSDK::AssetBuilderPattern(
            R"((.*sounds\/bopaudio\/events\/).*\.audioeventsource)",
            AssetBuilderSDK::AssetBuilderPattern::PatternType::Regex));

        builderDescriptor.m_patterns.push_back(AssetBuilderSDK::AssetBuilderPattern(
            R"((.*sounds\/bopaudio\/events\/).*\.audioevent)",
            AssetBuilderSDK::AssetBuilderPattern::PatternType::Regex));

        builderDescriptor.m_busId = azrtti_typeid<AudioEventAssetBuilderWorker>();
        builderDescriptor.m_version = 0;
        builderDescriptor.m_createJobFunction =
            [ObjectPtr = &m_eventBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->CreateJobs(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };
        builderDescriptor.m_processJobFunction =
            [ObjectPtr = &m_eventBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->ProcessJob(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };

        m_eventBuilder.BusConnect(builderDescriptor.m_busId);

        AssetBuilderSDK::AssetBuilderBus::Broadcast(
            &AssetBuilderSDK::AssetBuilderBus::Events::RegisterBuilderInformation,
            builderDescriptor);
    }

    void BopAudioAssetBuilderComponent::ConfigureAudioControlBuilder()
    {
        // Register Audio Control builder
        AssetBuilderSDK::AssetBuilderDesc builderDescriptor;
        builderDescriptor.m_name = "Audio Control Builder";
        // pattern finds all Audio Control xml files in the libs/gameaudio
        // folder and any of its subfolders.
        builderDescriptor.m_patterns.push_back(AssetBuilderSDK::AssetBuilderPattern(
            R"((.*libs\/gameaudio\/).*\.xml)",
            AssetBuilderSDK::AssetBuilderPattern::PatternType::Regex));
        builderDescriptor.m_busId = azrtti_typeid<AudioControlBuilderWorker>();
        builderDescriptor.m_version = 2;
        builderDescriptor.m_createJobFunction =
            [ObjectPtr = &m_audioControlBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->CreateJobs(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };
        builderDescriptor.m_processJobFunction =
            [ObjectPtr = &m_audioControlBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->ProcessJob(
                std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };

        builderDescriptor.m_flags |= AssetBuilderSDK::AssetBuilderDesc::BF_DeleteLastKnownGoodProductOnFailure;

        m_audioControlBuilder.BusConnect(builderDescriptor.m_busId);

        AssetBuilderSDK::AssetBuilderBus::Broadcast(
            &AssetBuilderSDK::AssetBuilderBus::Events::RegisterBuilderInformation,
            builderDescriptor);
    }

} // namespace BopAudio
