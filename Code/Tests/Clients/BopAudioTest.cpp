#include "ATLEntityData.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngine.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/BopAudioBus.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/BootstrapFixture.h"
#include "Clients/SimpleProjectFixture.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"

using ::testing::NiceMock;

namespace BopAudioTests
{

    struct ValidActivateArgs
    {
        static constexpr auto m_validAudioObjectId{ BopAudio::AudioObjectId{
            AZ_CRC_CE("testValidAudioObject") } };
        static constexpr auto m_validAudioEventId{ BopAudio::AudioEventId{
            AZ_CRC_CE("exploding") } };

        [[nodiscard]] static auto GetTriggerData() -> BopAudio::SATLTriggerImplData_BopAudio
        {
            auto implTriggerData{ decltype(GetTriggerData()){} };
            implTriggerData.SetImplAudioObjectId(m_validAudioObjectId);
            implTriggerData.SetImplEventId(m_validAudioEventId);

            return implTriggerData;
        }

        [[nodiscard]] static auto GetEventData() -> BopAudio::SATLEventData_BopAudio
        {
            auto implEventData{ decltype(GetEventData()){
                static_cast<AZ::u32>(m_validAudioObjectId) } };

            implEventData.SetImplEventId(m_validAudioEventId);

            return implEventData;
        }
    };

    TEST_F(BaseAudioTestFixture, SANITY_CHECK)
    {
    }

    TEST_F(BootstrapFixture, ASI_SANITY_CHECK)
    {
        EXPECT_EQ(BopAudio::SoundEngine::Get(), nullptr);
        EXPECT_FALSE(BopAudio::BopAudioRequestBus::HasHandlers());
        EXPECT_FALSE(Audio::Gem::EngineRequestBus::HasHandlers());
    }

    TEST_F(BootstrapFixture, Bootstrap_InitShutdown_CleanSuccess)
    {
        BopAudio::MiniAudioEngine const soundEngine{};
        EXPECT_NE(BopAudio::SoundEngine::Get(), nullptr);

        EXPECT_FALSE(Audio::AudioSystemImplementationRequestBus::HasHandlers());
        EXPECT_FALSE(Audio::AudioSystemImplementationNotificationBus::HasHandlers());
        BopAudio::AudioSystemImpl_miniaudio asiImpl{ "linux" };
        EXPECT_TRUE(Audio::AudioSystemImplementationNotificationBus::HasHandlers());
        EXPECT_TRUE(Audio::AudioSystemImplementationRequestBus::HasHandlers());

        EXPECT_EQ(asiImpl.Initialize(), Audio::EAudioRequestStatus::Success);
        EXPECT_EQ(asiImpl.ShutDown(), Audio::EAudioRequestStatus::Success);
    }

    TEST_F(BootstrapFixture, Bootstrap_Configuration_FileAliasesAreValid)
    {
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::BanksAlias), nullptr);
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::EventsAlias), nullptr);
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::SoundsAlias), nullptr);
    }

    TEST_F(BootstrapFixture, DISABLED_Bootstrap_PostInit_InitBankIsLoaded)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(BootstrapFixture, SoundBankAssetHandler_IsConnected)
    {
        EXPECT_EQ(
            AZ::AssetTypeInfoBus::GetNumOfEventHandlers(
                AZ::AzTypeInfo<BopAudio::SoundBankAsset>::Uuid()),
            1);
    }

    TEST_F(BootstrapFixture, AudioEventAssetAssetHandler_IsConnected)
    {
        EXPECT_EQ(
            AZ::AssetTypeInfoBus::GetNumOfEventHandlers(
                AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid()),
            1);
    }

    TEST_F(SimpleProjectFixture, SANITY_CHECK)
    {
        bool const soundBankAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::SoundBankAsset>::Uuid());
        EXPECT_TRUE(soundBankAssetHandlerConnected);

        bool const audioEventAssetHandlerConnected =
            AZ::AssetTypeInfoBus::HasHandlers(AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid());
        EXPECT_TRUE(audioEventAssetHandlerConnected);

        EXPECT_NE(BopAudio::SoundEngine::Get(), nullptr);
        EXPECT_NE(BopAudio::AsiInterface::Get(), nullptr);
    }

    TEST_F(
        SimpleProjectFixture, DISABLED_AudioEvents_ActivateEventContainingPlaySoundEvent_SoundPlays)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(SimpleProjectFixture, AsiTriggerActivation_GoodArgs_RequestsSucceed)
    {
        static constexpr Audio::SATLSourceData validSourceData{};

        auto const validTriggerData{ ValidActivateArgs::GetTriggerData() };
        auto validEventData{ ValidActivateArgs::GetEventData() };
        auto validImplAudioObjData{ BopAudio::SATLAudioObjectData_BopAudio{
            static_cast<AZ::u32>(ValidActivateArgs::m_validAudioObjectId),
            ValidActivateArgs::m_validAudioObjectId,
            "testObject" } };

        auto activateWithValidArgsResult = BopAudio::AsiInterface::Get()->ActivateTrigger(
            &validImplAudioObjData, &validTriggerData, &validEventData, &validSourceData);

        EXPECT_EQ(activateWithValidArgsResult, Audio::EAudioRequestStatus::Success);
    }

    TEST_F(SimpleProjectFixture, AsiTriggerActivation_BadArgs_RequestsFail)
    {
        AZ_TEST_START_TRACE_SUPPRESSION;
        bool const activateWithAllNullptrArgResult = []() -> bool
        {
            return BopAudio::AsiInterface::Get()->ActivateTrigger(
                       nullptr, nullptr, nullptr, nullptr) == Audio::EAudioRequestStatus::Success;
        }();
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);
        EXPECT_FALSE(activateWithAllNullptrArgResult);

        static constexpr Audio::SATLSourceData validSourceData{};

        bool const activateWithInvalidObjIdResult = []() -> bool
        {
            auto validTriggerData{ ValidActivateArgs::GetTriggerData() };
            auto validEventData{ ValidActivateArgs::GetEventData() };
            auto invalidImplAudioObjData{ BopAudio::SATLAudioObjectData_BopAudio{
                INVALID_AUDIO_OBJECT_ID } };

            return BopAudio::AsiInterface::Get()->ActivateTrigger(
                       &invalidImplAudioObjData,
                       &validTriggerData,
                       &validEventData,
                       &validSourceData) == Audio::EAudioRequestStatus::Success;
        }();
        EXPECT_FALSE(activateWithInvalidObjIdResult);
    }

} // namespace BopAudioTests
