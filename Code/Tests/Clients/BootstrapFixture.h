#pragma once

#include "Clients/BaseTestFixture.h"
#include "Engine/AudioSystemImpl_BopAudio.h"
#include "Engine/MiniAudioEngine.h"

namespace BopAudioTests
{
    class BootstrapFixture : public BaseAudioTestFixture
    {
    public:
        void SetUp() override;

        void TearDown() override;

    private:
        AZStd::optional<BopAudio::MiniAudioEngine> m_miniAudioEngine;
        AZStd::optional<BopAudio::AudioSystemImpl_miniaudio> m_miniAudioAsi;
    };
} // namespace BopAudioTests
