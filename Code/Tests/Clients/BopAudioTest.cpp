
#include "Clients/BaseTestFixture.h"
#include "Engine/AudioSystemImpl_script.h"
#include "IAudioSystemImplementation.h"

using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace BopAudioTests
{
    TEST_F(BaseAudioTestFixture, SANITY_CHECK)
    {
        EXPECT_EQ(Audio::Gem::EngineRequestBus::GetTotalNumOfEventHandlers(), 1);
        // We don't initialize the O3DE AudioSystem in these tests, so we don't expect our impl to
        // be registered.
        EXPECT_EQ(Audio::AudioSystemImplementationRequestBus::GetTotalNumOfEventHandlers(), 0);

        AZStd::optional<BopAudio::AudioSystemImpl_script> audioSystemImpl{};

        audioSystemImpl.emplace();

        AZStd::string const implName = []() -> decltype(implName)
        {
            auto result{ decltype(implName){} };
            Audio::AudioSystemImplementationRequestBus::BroadcastResult(
                result, &Audio::AudioSystemImplementationRequests::GetImplementationNameString);
            return result;
        }();


        EXPECT_TRUE(AZ::StringFunc::Equal(implName, BopAudio::AudioSystemImpl_script::ImplName))
            << "ImplName should be '" << BopAudio::AudioSystemImpl_script::ImplName
            << "', but it was '" << implName.c_str() << "'.";

       EXPECT_EQ(audioSystemImpl->Initialize(), Audio::EAudioRequestStatus::Success); 
       EXPECT_EQ(audioSystemImpl->ShutDown(), Audio::EAudioRequestStatus::Success);

        
        audioSystemImpl.reset();
    }
} // namespace BopAudioTests
