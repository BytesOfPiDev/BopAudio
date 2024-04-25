#include "Clients/BaseTestFixture.h"

#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Module/ModuleManagerBus.h"
#include "AzCore/Script/ScriptSystemComponent.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "AzCore/UserSettings/UserSettingsComponent.h"
#include "AzFramework/Application/Application.h"
#include "AzFramework/FileTag/FileTagComponent.h"
#include "AzFramework/Input/System/InputSystemComponent.h"
#include "AzFramework/PaintBrush/PaintBrushSystemComponent.h"
#include "AzFramework/Physics/Material/PhysicsMaterialSystemComponent.h"
#include "AzFramework/Render/GameIntersectorComponent.h"
#include "AzFramework/StreamingInstall/StreamingInstall.h"

#include "Clients/AudioEventAssetHandler.h"
#include "Clients/BopAudioSystemComponent.h"
#include "Clients/SoundBankAsset.h"
#include "Clients/SoundBankAssetHandler.h"
#include <AzCore/Slice/SliceSystemComponent.h>
#include <AzFramework/Asset/CustomAssetTypeComponent.h>
#include <AzFramework/Spawnable/SpawnableSystemComponent.h>

namespace BopAudioTests
{
    void AsiApplication::StartCommon(AZ::Entity* systemEntity)
    {
        AzFramework::Application::StartCommon(systemEntity);
    }

    void AsiApplication::RegisterCoreComponents()
    {
        AzFramework::Application::RegisterCoreComponents();

        RegisterComponentDescriptor(BopAudio::BopAudioSystemComponent::CreateDescriptor());
    }

    auto AsiApplication::GetRequiredSystemComponents() const -> AZ::ComponentTypeList
    {
        auto required = AzFramework::Application::GetRequiredSystemComponents();
        AZStd::erase(required, azrtti_typeid<AZ::ScriptSystemComponent>());

        AZStd::erase(required, azrtti_typeid<AzFramework::FileTag::ExcludeFileComponent>());
        AZStd::erase(
            required, azrtti_typeid<AzFramework::RenderGeometry::GameIntersectorComponent>());
        AZStd::erase(required, azrtti_typeid<AzFramework::InputSystemComponent>());
        AZStd::erase(required, azrtti_typeid<AzFramework::PaintBrushSystemComponent>());
        AZStd::erase(
            required,
            azrtti_typeid<AzFramework::StreamingInstall::StreamingInstallSystemComponent>());
        AZStd::erase(required, azrtti_typeid<Physics::MaterialSystemComponent>());

        // NOTE: CustomAssetTypeComponent has XML schema handler.
        AZStd::erase(required, azrtti_typeid<AzFramework::CustomAssetTypeComponent>());

        AZStd::erase(required, azrtti_typeid<AzFramework::SpawnableSystemComponent>());

        return required;
    }

    void BaseAudioTestFixture::SetUp()
    {
        AZ::ComponentApplication::Descriptor appDesc;

        appDesc.m_modules.push_back({ "MiniAudio" });

        appDesc.m_enableScriptReflection = false;

        AZ::ComponentApplication::StartupParameters startupParams{};
        startupParams.m_loadAssetCatalog = true;

        m_app.Start(appDesc, startupParams);

        BopAudio::BopAudioSystemComponent::RegisterFileAliases();
        m_eventAssetHandler.Register();
        m_soundBankAssetHandler.Register();

        AZ::UserSettingsComponentRequestBus::Broadcast(
            &AZ::UserSettingsComponentRequests::DisableSaveOnFinalize);

        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);
        AZ_TEST_ASSERT(serializeContext != nullptr);
    }

    void BaseAudioTestFixture::TearDown()
    {
        m_eventAssetHandler.Unregister();
        m_soundBankAssetHandler.Unregister();
        m_app.Stop();
    }

    void BaseAudioTestFixture::AssertHandlersConnected()
    {
        bool const soundBankAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::SoundBankAsset>::Uuid());
        AZ_TEST_ASSERT(soundBankAssetHandlerConnected);

        bool const audioEventAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid());
        AZ_TEST_ASSERT(audioEventAssetHandlerConnected);
    }

} // namespace BopAudioTests
