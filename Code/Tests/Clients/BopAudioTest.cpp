#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/Outcome/Outcome.h"
#include "Clients/MockSoundEngine.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/BopAudioBus.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/BootstrapFixture.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"

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
            auto const implTriggerData{ decltype(GetTriggerData()){ GetAudioEventId() } };

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
        EXPECT_TRUE(Audio::AudioSystemImplementationNotificationBus::HasHandlers());
        EXPECT_TRUE(Audio::AudioSystemImplementationRequestBus::HasHandlers());

        EXPECT_CALL(soundEngine, Initialize).Times(1).WillOnce(Return(AZ::Success()));
        EXPECT_CALL(soundEngine, Shutdown).Times(1).WillOnce(Return(AZ::Success()));
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

    TEST_F(BootstrapFixture, AudioEventAssetAssetHandler_IsConnected)
    {
        EXPECT_EQ(
            AZ::AssetTypeInfoBus::GetNumOfEventHandlers(
                AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid()),
            1);
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

        static auto const someNameAsImplEventId{ BopAudio::AudioEventId{ someName } };
        static auto const someOtherNameAsImplEventId{ BopAudio::AudioEventId{ someOtherName } };
        static auto const someNameAsAtlEventId{ Audio::AudioStringToID<Audio::TAudioEventID>(
            someName) };

        EXPECT_TRUE(
            static_cast<Audio::TAudioEventID>(someNameAsImplEventId) == someNameAsAtlEventId);
        EXPECT_FALSE(
            static_cast<Audio::TAudioEventID>(someOtherNameAsImplEventId) == someNameAsAtlEventId);

        EXPECT_FALSE(
            static_cast<Audio::TAudioEventID>(someNameAsImplEventId) != someNameAsAtlEventId);
        EXPECT_TRUE(
            static_cast<Audio::TAudioEventID>(someOtherNameAsImplEventId) != someNameAsAtlEventId);
    }

} // namespace BopAudioTests
