#include "Clients/BaseTestFixture.h"

#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/Module/ModuleManagerBus.h"
#include "AzCore/Script/ScriptSystemComponent.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "AzCore/UserSettings/UserSettingsComponent.h"
#include "AzFramework/Application/Application.h"
#include "AzFramework/FileTag/FileTagComponent.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "AzFramework/Input/System/InputSystemComponent.h"
#include "AzFramework/PaintBrush/PaintBrushSystemComponent.h"
#include "AzFramework/Physics/Material/PhysicsMaterialSystemComponent.h"
#include "AzFramework/Render/GameIntersectorComponent.h"
#include "AzFramework/StreamingInstall/StreamingInstall.h"

#include "Clients/SoundBankAsset.h"

namespace BopAudioTests
{
    void AsiApplication::StartCommon(AZ::Entity* systemEntity)
    {
        AzFramework::Application::StartCommon(systemEntity);
    }

    void AsiApplication::RegisterCoreComponents()
    {
        AzFramework::Application::RegisterCoreComponents();
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

        return required;
    }

    void BaseAudioTestFixture::SetUp()
    {
        m_prevFileIO = AZ::IO::FileIOBase::GetInstance();
        if (m_prevFileIO)
        {
            AZ::IO::FileIOBase::SetInstance(nullptr);
        }

        // Replace with a new LocalFileIO...
        m_fileIO = AZStd::make_unique<AZ::IO::LocalFileIO>();
        AZ::IO::FileIOBase::SetInstance(m_fileIO.get());

        AZ::ComponentApplication::Descriptor appDesc;

        appDesc.m_modules.push_back({ "AudioSystem" });
        appDesc.m_modules.push_back({ "BopAudio" });

        appDesc.m_enableScriptReflection = false;

        AZ::ComponentApplication::StartupParameters startupParams{};

        m_app.Start(appDesc);

        AZ::UserSettingsComponentRequestBus::Broadcast(
            &AZ::UserSettingsComponentRequests::DisableSaveOnFinalize);

        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);
        AZ_TEST_ASSERT(serializeContext != nullptr);

        bool const soundBankAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::SoundBankAsset>::Uuid());
        ASSERT_TRUE(soundBankAssetHandlerConnected);

        bool const audioEventAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid());
        ASSERT_TRUE(audioEventAssetHandlerConnected);
    }

    void BaseAudioTestFixture::TearDown()
    {
        m_app.Stop();
        m_fileIO.reset();

        AZ::IO::FileIOBase::SetInstance(nullptr);
        if (m_prevFileIO)
        {
            AZ::IO::FileIOBase::SetInstance(m_prevFileIO);
            m_prevFileIO = nullptr;
        }
    }

} // namespace BopAudioTests
