#include "ATLEntityData.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "Clients/MockSoundEngine.h"
#include "Engine/AudioEngineEventBus.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/BopAudioBus.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/BootstrapFixture.h"
#include "Clients/MockAudioEventAsset.h"
#include "Clients/SimpleProjectFixture.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/MiniAudioEngineBus.h"
#include <AzCore/Outcome/Outcome.h>

using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace BopAudioTests
{
    struct ValidActivateArgs
    {
        static auto GetAudioObjectId() -> BopAudio::AudioObjectId
        {
            return BopAudio::AudioObjectId{ Audio::AudioStringToID<BopAudio::AudioObjectId>(
                "testValidAudioObject") };
        }
        static auto GetAudioEventId() -> BopAudio::AudioEventId
        {
            return Audio::AudioStringToID<BopAudio::AudioEventId>("testValidAudioObject");
        }

        [[nodiscard]] static auto GetTriggerData() -> BopAudio::SATLTriggerImplData_BopAudio
        {
            auto const implTriggerData{ decltype(GetTriggerData()){GetAudioEventId()} };

            return implTriggerData;
        }

        [[nodiscard]] static auto GetEventData() -> BopAudio::SATLEventData_BopAudio
        {
            auto implEventData{ decltype(GetEventData()){ GetAudioEventId() } };

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

    TEST_F(BootstrapFixture, Asi_InitAfterBootstrap_ShutdownSuccess)
    {
        MockSoundEngine soundEngine{};
        EXPECT_NE(BopAudio::SoundEngine::Get(), nullptr);

        EXPECT_FALSE(Audio::AudioSystemImplementationRequestBus::HasHandlers());
        EXPECT_FALSE(Audio::AudioSystemImplementationNotificationBus::HasHandlers());
        BopAudio::AudioSystemImpl_miniaudio asiImpl{ "linux" };
        EXPECT_TRUE(Audio::AudioSystemImplementationNotificationBus::HasHandlers());
        EXPECT_TRUE(Audio::AudioSystemImplementationRequestBus::HasHandlers());

        EXPECT_CALL(soundEngine, Initialize).Times(1).WillOnce(Return(AZ::Success()));
        EXPECT_CALL(soundEngine, Shutdown).Times(1).WillOnce(Return(AZ::Success()));

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

        MockAudioEventAsset eventAsset{ ValidActivateArgs::GetAudioEventId() };
        eventAsset.RegisterAudioEvent();
        EXPECT_TRUE(eventAsset.BusIsConnected());
        EXPECT_TRUE(eventAsset.BusIsConnectedId(ValidActivateArgs::GetAudioEventId()));

        EXPECT_CALL(eventAsset, TryStartEvent)
            .Times(1)
            .WillOnce(Return(
                [](BopAudio::AudioObject const&) -> bool
                {
                    return true;
                }));

        auto const validTriggerData{ ValidActivateArgs::GetTriggerData() };
        auto validEventData{ ValidActivateArgs::GetEventData() };
        auto validImplAudioObjData{ BopAudio::SATLAudioObjectData_BopAudio{
            static_cast<Audio::TAudioObjectID>(ValidActivateArgs::GetAudioObjectId()),
            ValidActivateArgs::GetAudioObjectId(),
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

        AZ_TEST_START_TRACE_SUPPRESSION;
        bool const activateWithInvalidObjIdResult = []() -> bool
        {
            auto validTriggerData{ ValidActivateArgs::GetTriggerData() };
            auto validEventData{ ValidActivateArgs::GetEventData() };
            auto invalidImplAudioObjData{ BopAudio::SATLAudioObjectData_BopAudio{
                INVALID_AUDIO_OBJECT_ID, BopAudio::GlobalAudioObjectId } };

            return BopAudio::AsiInterface::Get()->ActivateTrigger(
                       &invalidImplAudioObjData,
                       &validTriggerData,
                       &validEventData,
                       &validSourceData) == Audio::EAudioRequestStatus::Success;
        }();
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);
        EXPECT_FALSE(activateWithInvalidObjIdResult);
    }

    TEST(AudioEventIdComparisonTests, NullId_ComparedToNullAtlId_IsEqual)
    {
        static constexpr auto nullImplEventId{ BopAudio::InvalidAudioEventId };
        static constexpr auto nullAtlId{ INVALID_AUDIO_EVENT_ID };

        EXPECT_TRUE(static_cast<Audio::TATLIDType>(nullImplEventId) == nullAtlId);
        EXPECT_FALSE(static_cast<Audio::TATLIDType>(nullImplEventId) != nullAtlId);
    }

    TEST(ImplAudioObjectDataTests, ContructWithValidIds_CallArgGetters_ReturnsGivenIds)
    {
        constexpr auto objectIdName{ "oh_my_these_chocodiles" };
        static auto const givenImplObjectId{ Audio::AudioStringToID<BopAudio::AudioObjectId>(
            objectIdName) };
        static auto const givenAtlObjectId{ Audio::AudioStringToID<Audio::TAudioObjectID>(
            objectIdName) };

        BopAudio::SATLAudioObjectData_BopAudio const obj{ givenAtlObjectId, givenImplObjectId };

        EXPECT_EQ(obj.GetImplAudioObjectId(), givenImplObjectId);
        EXPECT_EQ(obj.GetAtlAudioObjectId(), givenAtlObjectId);
    }
    
    TEST(AudioEventIdCreationTests, CreateAudioEventId_CompareToAtlCreatedId_CorrectlyCompares)
    {
        static constexpr auto someName{ "something_something_something_darkside" };
        static constexpr auto someOtherName{ "something_something_something_darksid" };

        static auto const someNameAsImplEventId{BopAudio::AudioEventId{someName}};
        static auto const someOtherNameAsImplEventId{BopAudio::AudioEventId{someOtherName}};
        static auto const someNameAsAtlEventId{ Audio::AudioStringToID<Audio::TAudioEventID>(someName) };

        EXPECT_TRUE(static_cast<Audio::TAudioEventID>(someNameAsImplEventId) == someNameAsAtlEventId);
        EXPECT_FALSE(static_cast<Audio::TAudioEventID>(someOtherNameAsImplEventId) == someNameAsAtlEventId);

        EXPECT_FALSE(static_cast<Audio::TAudioEventID>(someNameAsImplEventId) != someNameAsAtlEventId);
        EXPECT_TRUE(static_cast<Audio::TAudioEventID>(someOtherNameAsImplEventId) != someNameAsAtlEventId);
    }

} // namespace BopAudioTests
