#pragma once

#include <AzCore/UnitTest/UnitTest.h>
#include <AzTest/AzTest.h>

#include "AzFramework/Application/Application.h"
#include "IAudioSystem.h"

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

        auto GetAudioSystem() -> Audio::IAudioSystem*
        {
            return AZ::Interface<Audio::IAudioSystem>::Get();
        }

        auto GetApp() -> AsiApplication&
        {
            return m_app;
        }

    private:
        AsiApplication m_app;
    };
} // namespace BopAudioTests
