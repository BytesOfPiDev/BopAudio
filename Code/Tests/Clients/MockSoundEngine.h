#pragma once

#include <AzTest/AzTest.h>

#include "Engine/MiniAudioEngineBus.h"

namespace BopAudioTests
{
    class MockSoundEngine : public BopAudio::SoundEngine::Registrar
    {
    public:
        MOCK_METHOD0(Initialize, BopAudio::NullOutcome(void));
        MOCK_METHOD0(Shutdown, BopAudio::NullOutcome(void));

        MOCK_METHOD1(RegisterFile, BopAudio::NullOutcome(BopAudio::RegisterFileData const&));
        MOCK_METHOD0(CreateAudioObject, BopAudio::AudioObjectId(void));
        MOCK_METHOD1(RemoveAudioObject, void(BopAudio::AudioObjectId));
        MOCK_METHOD1(StartEvent, BopAudio::NullOutcome(BopAudio::StartEventData const&));
        MOCK_METHOD1(StopEvent, bool(BopAudio::AudioEventId eventId));

        MOCK_METHOD0(GetSoundEngine, ma_engine*(void));
        MOCK_METHOD1(LoadSound, BopAudio::NullOutcome(BopAudio::SoundRef const& resourceRef));
        MOCK_CONST_METHOD0(IsInit, bool());
    };
} // namespace BopAudioTests
