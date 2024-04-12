#pragma once

#include <AzTest/AzTest.h>

#include "Clients/AudioEventAsset.h"
#include "Engine/AudioEventBus.h"
#include "Engine/AudioObject.h"

namespace BopAudioTests
{
    class MockAudioEventAsset : public BopAudio::AudioEventAsset
    {
    public:
        MOCK_METHOD1(TryStartEvent, bool(BopAudio::AudioObject& obj));
        MOCK_METHOD1(TryStopEvent, bool(BopAudio::AudioObject& obj));
        MOCK_CONST_METHOD0(GetEventState, BopAudio::AudioEventState(void));
    };
} // namespace BopAudioTests
