#include "Clients/SimpleProjectFixture.h"

#include "Clients/BootstrapFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/MiniAudioEngine.h"
#include <IAudioInterfacesCommonData.h>

namespace BopAudioTests
{
    void SimpleProjectFixture::SetUp()
    {
        BootstrapFixture::SetUp();

        m_asiImpl = AZStd::make_unique<BopAudio::AudioSystemImpl_miniaudio>("linux");
        m_soundEngine = AZStd::make_unique<BopAudio::MiniAudioEngine>();

        ASSERT_EQ(m_asiImpl->Initialize(), Audio::EAudioRequestStatus::Success);
    }

    void SimpleProjectFixture::TearDown()
    {
        m_soundEngine.reset();
        m_asiImpl.reset();

        BootstrapFixture::TearDown();
    }
} // namespace BopAudioTests
