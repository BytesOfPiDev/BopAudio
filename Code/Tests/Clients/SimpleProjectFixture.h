#pragma once

#include "Clients/BootstrapFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/MiniAudioEngine.h"

namespace BopAudioTests
{
    class SimpleProjectFixture : public BootstrapFixture
    {
    public:
        void SetUp() override;

        void TearDown() override;

    private:
        AZStd::unique_ptr<BopAudio::MiniAudioEngine> m_soundEngine;
        AZStd::unique_ptr<BopAudio::AudioSystemImpl_miniaudio> m_asiImpl;
    };
} // namespace BopAudioTests
