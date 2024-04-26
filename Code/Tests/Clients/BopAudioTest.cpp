#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

#include "Clients/AudioEventAsset.h"
#include "Clients/BootstrapFixture.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioObject.h"
#include "Engine/Id.h"

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
