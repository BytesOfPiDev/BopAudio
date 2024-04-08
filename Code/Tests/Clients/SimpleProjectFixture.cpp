#include "Clients/SimpleProjectFixture.h"

#include "Clients/BootstrapFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/MiniAudioEngine.h"

namespace BopAudioTests
{
    void SimpleProjectFixture::SetUp()
    {
        BootstrapFixture::SetUp();

        m_asiImpl = AZStd::make_unique<BopAudio::AudioSystemImpl_miniaudio>("linux");
        m_soundEngine = AZStd::make_unique<BopAudio::MiniAudioEngine>();

        m_asiImpl->Initialize();
    }

    void SimpleProjectFixture::TearDown()
    {
        m_soundEngine.reset();
        m_asiImpl.reset();

        BootstrapFixture::TearDown();
    }
} // namespace BopAudioTests
