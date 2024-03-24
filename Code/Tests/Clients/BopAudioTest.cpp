#include "ATLEntityData.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "Engine/Id.h"
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
    TEST_F(BootstrapFixture, ASI_SANITY_CHECK)
    {
        EXPECT_NE(AZ::Interface<Audio::IAudioSystem>::Get(), nullptr);
        EXPECT_NE(BopAudio::AudioEngineInterface::Get(), nullptr);

        EXPECT_NE(BopAudio::AudioEngineInterface::Get(), nullptr);
        EXPECT_EQ(BopAudio::BopAudioRequestBus::GetTotalNumOfEventHandlers(), 1);
        EXPECT_EQ(Audio::Gem::EngineRequestBus::GetTotalNumOfEventHandlers(), 1);
    }

    TEST_F(BootstrapFixture, Bootstrap_Configuration_FileAliasesAreValid)
    {
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::BanksAlias), nullptr);
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::EventsAlias), nullptr);
        EXPECT_NE(AZ::IO::FileIOBase::GetInstance()->GetAlias(BopAudio::SoundsAlias), nullptr);
    }

    TEST_F(BootstrapFixture, Bootstrap_PostInit_InitBankIsLoaded)
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
    }

    TEST_F(SimpleProjectFixture, Asi_ActivateEvent_IsSuccess)
    {
        Audio::EAudioRequestStatus result{};
        AZ_TEST_START_TRACE_SUPPRESSION;
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result,
            &Audio::AudioSystemImplementationRequests::ActivateTrigger,
            nullptr,
            nullptr,
            nullptr,
            nullptr);
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

        EXPECT_EQ(result, Audio::EAudioRequestStatus::Success);
    }

    TEST_F(SimpleProjectFixture, AudioEvents_ActivateEventContainingPlaySoundEvent_SoundPlays)
    {
        EXPECT_TRUE(false);
    }

    TEST_F(SimpleProjectFixture, RequestActivate_BadActivateArgs_RequestFails)
    {
        AZ_TEST_START_TRACE_SUPPRESSION;
        bool const activateWithAllNullptrArgResult = []() -> bool
        {
            return BopAudio::AsiInterface::Get()->ActivateTrigger(
                       nullptr, nullptr, nullptr, nullptr) == Audio::EAudioRequestStatus::Success;
        }();
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);
        EXPECT_FALSE(activateWithAllNullptrArgResult);

        static constexpr auto validAudioObjectId{ BopAudio::AudioObjectId{ AZ_CRC_CE("test") } };

        BopAudio::SATLTriggerImplData_BopAudio const validImplTriggerData =
            []() -> decltype(validImplTriggerData)
        {
            auto implTriggerData{ decltype(validImplTriggerData){} };
            implTriggerData.SetImplAudioObjectId(validAudioObjectId);

            return implTriggerData;
        }();

        BopAudio::SATLEventData_BopAudio validImplEventData = []() -> decltype(validImplEventData)
        {
            auto implEventData{ decltype(validImplEventData){
                static_cast<AZ::u32>(validAudioObjectId) } };

            return implEventData;
        }();

        Audio::SATLSourceData const validAtlSourceData{};

        bool const activateWithInvalidObjIdResult =
            [&validImplTriggerData, &validImplEventData, &validAtlSourceData]() -> bool
        {
            auto invalidImplAudioObjData{ BopAudio::SATLAudioObjectData_BopAudio{
                INVALID_AUDIO_OBJECT_ID } };

            return BopAudio::AsiInterface::Get()->ActivateTrigger(
                       &invalidImplAudioObjData,
                       &validImplTriggerData,
                       &validImplEventData,
                       &validAtlSourceData) == Audio::EAudioRequestStatus::Success;
        }();
        EXPECT_FALSE(activateWithInvalidObjIdResult);
    }

} // namespace BopAudioTests
