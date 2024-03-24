#include "AzTest/AzTest.h"

#include "AzCore/UnitTest/TestTypes.h"
#include "Clients/AsiTestEnvironment.h"

AZ_UNIT_TEST_HOOK(DEFAULT_UNIT_TEST_ENV);

namespace BopAudioTests
{
    class BaseAudioTestFixture : public UnitTest::LeakDetectionFixture
    {
    };

    class BootstrapFixture : public BaseAudioTestFixture
    {
    };

    class InitializedAsiFixture : public BaseAudioTestFixture
    {
    };

    class SimpleProjectFixture : public UnitTest::LeakDetectionFixture
    {
    };

    TEST_F(BootstrapFixture, ASI_SANITY_CHECK)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_ConfigFile_FileExists)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_ConfigFile_JsonSuccessfullyValidates)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_Configuration_FileAliasesAreValid)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_Configuration_SystemBusHandlerIsConnected)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_InitAsi_IsSuccess)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_PostInit_InitBankIsLoaded)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, Bootstrap_PostInit_MiniAudioEngineBusHandlerIsConnected)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(InitializedAsiFixture, SoundBankAssetHandler_IsConnected)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(InitializedAsiFixture, SoundEventAssetAssetHandler_IsConnected)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(SimpleProjectFixture, AudioEvents_ActivateEvent_EngineReceivesRequest)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(SimpleProjectFixture, AudioEvents_ActivateEventContainingPlaySoundEvent_SoundPlays)
    {
        EXPECT_TRUE(false);
    }

} // namespace BopAudioTests
