#include "Clients/BootstrapFixture.h"

#include "Engine/MiniAudioEngineBus.h"
#include "IAudioSystemImplementation.h"

namespace BopAudioTests
{
    void BootstrapFixture::SetUp()
    {
        BaseAudioTestFixture::SetUp();

        // We will create the engine, so there should not be one already registered.
        ASSERT_EQ(BopAudio::AudioEngineInterface::Get(), nullptr);
        // We will create the implementation, so there should not be one already registered.
        ASSERT_EQ(Audio::AudioSystemImplementationRequestBus::GetTotalNumOfEventHandlers(), 0);

        m_miniAudioAsi.emplace("linux");
        m_miniAudioEngine.emplace();

        ASSERT_NE(BopAudio::AudioEngineInterface::Get(), nullptr);
        ASSERT_EQ(Audio::AudioSystemImplementationRequestBus::GetTotalNumOfEventHandlers(), 1);
    }

    void BootstrapFixture::TearDown()
    {
        BaseAudioTestFixture::TearDown();
    }
} // namespace BopAudioTests
