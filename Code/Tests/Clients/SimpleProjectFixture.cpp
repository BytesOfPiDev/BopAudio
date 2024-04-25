#include "Clients/SimpleProjectFixture.h"

#include "Clients/BootstrapFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include <IAudioInterfacesCommonData.h>

namespace BopAudioTests
{
    void SimpleProjectFixture::SetUp()
    {
        BootstrapFixture::SetUp();

        m_asiImpl = AZStd::make_unique<BopAudio::AudioSystemImpl_miniaudio>("linux");

        ASSERT_EQ(m_asiImpl->Initialize(), Audio::EAudioRequestStatus::Success);
    }

    void SimpleProjectFixture::TearDown()
    {
        m_asiImpl.reset();

        BootstrapFixture::TearDown();
    }
} // namespace BopAudioTests
