#pragma once

#include <AzCore/UnitTest/UnitTest.h>
#include <AzTest/AzTest.h>

#include "AzFramework/Application/Application.h"
#include "Clients/AudioEventAssetHandler.h"

namespace BopAudioTests
{
    class AsiApplication : public AzFramework::Application
    {
    public:
        AZ_DISABLE_COPY_MOVE(AsiApplication);

        AsiApplication() = default;
        ~AsiApplication() override = default;

        void StartCommon(AZ::Entity* systemEntity) override;

        void RegisterCoreComponents() override;
        auto GetRequiredSystemComponents() const -> AZ::ComponentTypeList override;
    };

    class BaseAudioTestFixture
        : public UnitTest::TraceBusRedirector
        , public testing::Test
    {
    public:
        void SetUp() override;
        void TearDown() override;

    protected:
        virtual void AssertHandlersConnected() final;

    private:
        AsiApplication m_app;
        AZStd::string m_configFilePath;
        BopAudio::AudioEventAssetHandler m_eventAssetHandler;
    };
} // namespace BopAudioTests
