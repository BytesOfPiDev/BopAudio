#include <AzTest/AzTest.h>

#include "ATLEntityData.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "AzCore/XML/rapidxml.h"
#include "Clients/Mocks/MockEventData.h"
#include "Clients/Mocks/MockTriggerImplData.h"
#include "Clients/ParameterTypes.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"

#include "Clients/BaseTestFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Mocks/MockAudioObjectData.h"

using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

static constexpr auto ValidObjectName{ "roger_smith" };
static constexpr auto ValidObjectId1{ Audio::TAudioObjectID{ 1 } };
static constexpr auto ValidEventId1{ Audio::TAudioObjectID{ 1 } };

TEST(StartAudioEventDataTests, Constructed_GetAudioEventId_ReturnsInvalidId)
{
    BopAudio::StartEventData constructedStartEventData{};

    EXPECT_EQ(constructedStartEventData.GetAudioEventId(), INVALID_AUDIO_EVENT_ID);
    EXPECT_NE(constructedStartEventData.GetAudioEventId(), Audio::TAudioEventID{ ValidEventId1 });
}

TEST(StartAudioEventDataTests, HasValidEventId_GetAudioEventId_ReturnsTheValidId)
{
    BopAudio::StartEventData constructedStartEventData{ nullptr,
                                                        ValidEventId1,
                                                        INVALID_AUDIO_OBJECT_ID };

    EXPECT_EQ(constructedStartEventData.GetAudioEventId(), ValidEventId1);
    EXPECT_NE(constructedStartEventData.GetAudioEventId(), INVALID_AUDIO_EVENT_ID);
}

TEST(StopAudioEventDataTests, Constructed_GetAudioEventId_ReturnsInvalidId)
{
    BopAudio::StopEventData constructedStopEventData{};

    EXPECT_EQ(constructedStopEventData.GetAudioEventId(), INVALID_AUDIO_EVENT_ID);
    EXPECT_NE(constructedStopEventData.GetAudioEventId(), Audio::TAudioEventID{ ValidEventId1 });
}

TEST_F(BaseAudioTestFixture, SANITY_CHECK)
{
    EXPECT_NE(AZ::IO::FileIOBase::GetInstance(), nullptr);

    AZStd::string const implName = []() -> decltype(implName)
    {
        auto result{ decltype(implName){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result, &Audio::AudioSystemImplementationRequests::GetImplementationNameString);
        return result;
    }();

    EXPECT_TRUE(AZ::StringFunc::Equal(implName, BopAudio::AudioSystemImpl_bopaudio::ImplName))
        << "ImplName should be '" << BopAudio::AudioSystemImpl_bopaudio::ImplName
        << "', but it was '" << implName.c_str() << "'.";

    EXPECT_EQ(GetAudioSystemImpl().Initialize(), Audio::EAudioRequestStatus::Success);
    EXPECT_EQ(GetAudioSystemImpl().ShutDown(), Audio::EAudioRequestStatus::Success);
}

TEST_F(BaseAudioTestFixture, RuntimeSystemComponentActivated_TryResolveAliases_SuccessfullyResolves)
{
    auto* fileIo = AZ::IO::FileIOBase::GetInstance();
    EXPECT_TRUE(fileIo->ResolvePath(BopAudio::ProjectAlias).has_value());
    EXPECT_TRUE(fileIo->ResolvePath(BopAudio::EventsAlias).has_value());
}

TEST_F(BaseAudioTestFixture, AudioSystemImplIsUninitialized_InitializeAndShutdown_ReturnsSuccess)
{
    GetAudioSystemImpl().Initialize();

    EXPECT_EQ(GetAudioSystemImpl().Initialize(), Audio::EAudioRequestStatus::Success);
    EXPECT_EQ(GetAudioSystemImpl().ShutDown(), Audio::EAudioRequestStatus::Success);
}

struct InitializedAudioSystemImplFixture : public BaseAudioTestFixture
{
    AZ_DISABLE_COPY_MOVE(InitializedAudioSystemImplFixture);

    InitializedAudioSystemImplFixture() = default;
    ~InitializedAudioSystemImplFixture() override = default;

    void SetUp() override
    {
        GetAudioSystemImpl().Initialize();
    }

    void TearDown() override
    {
        GetAudioSystemImpl().ShutDown();
    }
};

TEST_F(InitializedAudioSystemImplFixture, RegisterAudioObject_PassValidDataAndName_ReturnsSuccess)
{
    MockAudioObjectData validAudioObject{};

    EXPECT_CALL(validAudioObject, GetName()).Times(1).WillOnce(Return(AZ::Name{ ValidObjectName }));
    EXPECT_CALL(validAudioObject, ChangeName({ ValidObjectName })).Times(1);

    Audio::EAudioRequestStatus const registerAudioObjectResult =
        [&validAudioObject]() -> decltype(registerAudioObjectResult)
    {
        auto result{ decltype(registerAudioObjectResult){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result,
            &Audio::AudioSystemImplementationRequests::RegisterAudioObject,
            &validAudioObject,
            ValidObjectName);
        return result;
    }();

    EXPECT_EQ(registerAudioObjectResult, Audio::EAudioRequestStatus::Success);
    EXPECT_EQ(validAudioObject.GetName(), AZ::Name{ ValidObjectName });
}

TEST_F(InitializedAudioSystemImplFixture, UnregisterAudioObject_PassValidData_ReturnsSuccess)
{
    MockAudioObjectData validAudioObject{};
    EXPECT_CALL(validAudioObject, ChangeName({ ValidObjectName })).Times(2);

    Audio::AudioSystemImplementationRequestBus::Broadcast(
        &Audio::AudioSystemImplementationRequests::RegisterAudioObject,
        &validAudioObject,
        ValidObjectName);

    Audio::EAudioRequestStatus const unregisterAudioObjectResult =
        [&validAudioObject]() -> decltype(unregisterAudioObjectResult)
    {
        auto result{ decltype(unregisterAudioObjectResult){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result,
            &Audio::AudioSystemImplementationRequests::UnregisterAudioObject,
            &validAudioObject);
        return result;
    }();

    EXPECT_EQ(unregisterAudioObjectResult, Audio::EAudioRequestStatus::Success);

    GetAudioSystemImpl().RegisterAudioObject(&validAudioObject, ValidObjectName);
}

TEST_F(InitializedAudioSystemImplFixture, ActivateTrigger_PassValidData_ReturnsSuccess)
{
    MockAudioObjectData validAudioObject{};
    MockTriggerImplData validTrigger{};
    MockAudioEventData validEvent{};
    Audio::SATLSourceData validSource{};

    EXPECT_CALL(validAudioObject, GetAtlAudioObjectId()).Times(1).WillOnce(Return(ValidObjectId1));

    Audio::EAudioRequestStatus const activateTriggerResult =
        [&]() -> decltype(activateTriggerResult)
    {
        auto result{ decltype(activateTriggerResult){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result,
            &Audio::AudioSystemImplementationRequests::ActivateTrigger,
            &validAudioObject,
            &validTrigger,
            &validEvent,
            &validSource);

        return result;
    }();

    EXPECT_EQ(activateTriggerResult, Audio::EAudioRequestStatus::Success);
}

TEST_F(InitializedAudioSystemImplFixture, NewAudioTriggerImpl_PassInvalidData_ReturnsNullptr)
{
    AZ::rapidxml::xml_document<char> doc{};

    AZ_TEST_START_TRACE_SUPPRESSION;
    Audio::IATLTriggerImplData* const newAudioTriggerWithNullParamResult =
        [&]() -> decltype(newAudioTriggerWithNullParamResult)
    {
        auto result{ decltype(newAudioTriggerWithNullParamResult){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result, &Audio::AudioSystemImplementationRequests::NewAudioTriggerImplData, nullptr);

        return result;
    }();
    AZ_TEST_STOP_TRACE_SUPPRESSION(1);

    EXPECT_EQ(newAudioTriggerWithNullParamResult, nullptr);

    AZ_TEST_START_TRACE_SUPPRESSION;
    [[maybe_unused]] auto* node{ doc.allocate_node(
        AZ::rapidxml::node_element, BopAudio::XmlTags::TriggerTag) };

    Audio::IATLTriggerImplData* newAudioTriggerWithBadNode =
        [&]() -> decltype(newAudioTriggerWithNullParamResult)
    {
        auto result{ decltype(newAudioTriggerWithNullParamResult){} };
        Audio::AudioSystemImplementationRequestBus::BroadcastResult(
            result, &Audio::AudioSystemImplementationRequests::NewAudioTriggerImplData, node);

        return result;
    }();
    AZ_TEST_STOP_TRACE_SUPPRESSION(1);

    EXPECT_EQ(newAudioTriggerWithBadNode, nullptr);
}
