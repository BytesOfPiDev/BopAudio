#include "Builder/BopAudioControlBuilderComponent.h"
#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/SerializeContext.h"

namespace BopAudio
{
    AZ_COMPONENT_IMPL(BopAudioControlBuilderComponent, "BopAudioBuilderComponent", "{B617F3AF-E51C-40BF-884F-60323F924121}");

    void BopAudioControlBuilderComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* const serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BopAudioControlBuilderComponent, AZ::Component>()->Version(0)->Attribute(
                AZ::Edit::Attributes::SystemComponentTags, AZStd::vector<AZ::Crc32>({ AssetBuilderSDK::ComponentTags::AssetBuilder }));
        }
    }

    BopAudioControlBuilderComponent::BopAudioControlBuilderComponent() = default;
    BopAudioControlBuilderComponent::~BopAudioControlBuilderComponent() = default;

    void BopAudioControlBuilderComponent::Init()
    {
    }

    void BopAudioControlBuilderComponent::Activate()
    {
        // Register Audio Control builder
        AssetBuilderSDK::AssetBuilderDesc builderDescriptor;
        builderDescriptor.m_name = "Audio Control Builder";
        // pattern finds all Audio Control xml files in the libs/gameaudio folder and any of its subfolders.
        builderDescriptor.m_patterns.push_back(AssetBuilderSDK::AssetBuilderPattern(
            R"((.*libs\/gameaudio\/).*\.xml)", AssetBuilderSDK::AssetBuilderPattern::PatternType::Regex));
        builderDescriptor.m_busId = azrtti_typeid<BopAudioControlBuilderWorker>();
        builderDescriptor.m_version = 2;
        builderDescriptor.m_createJobFunction = [ObjectPtr = &m_audioControlBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->CreateJobs(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };
        builderDescriptor.m_processJobFunction = [ObjectPtr = &m_audioControlBuilder](auto&& PH1, auto&& PH2)
        {
            ObjectPtr->ProcessJob(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        };

        // (optimization) this builder does not emit source dependencies:
        builderDescriptor.m_flags |= AssetBuilderSDK::AssetBuilderDesc::BF_EmitsNoDependencies;

        m_audioControlBuilder.BusConnect(builderDescriptor.m_busId);

        AssetBuilderSDK::AssetBuilderBus::Broadcast(
            &AssetBuilderSDK::AssetBuilderBus::Events::RegisterBuilderInformation, builderDescriptor);
    }

    void BopAudioControlBuilderComponent::Deactivate()
    {
        m_audioControlBuilder.BusDisconnect();
    }

    void BopAudioControlBuilderComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("BopAudioBuilderService"));
    }

    void BopAudioControlBuilderComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("BopAudioBuilderService"));
    }

    void BopAudioControlBuilderComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& /*required*/)
    {
    }

    void BopAudioControlBuilderComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& /*dependent*/)
    {
    }

} // namespace BopAudio
