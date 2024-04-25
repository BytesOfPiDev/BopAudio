#pragma once

#include "Clients/BootstrapFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"

namespace BopAudioTests
{
    class SimpleProjectFixture : public BootstrapFixture
    {
    public:
        void SetUp() override;
        void TearDown() override;

    private:
        AZStd::unique_ptr<BopAudio::AudioSystemImpl_miniaudio> m_asiImpl;
    };
} // namespace BopAudioTests
