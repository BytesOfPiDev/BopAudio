#pragma once

#include <AzTest/AzTest.h>

#include "Engine/ATLEntities_BopAudio.h"
#include "IAudioInterfacesCommonData.h"

class MockAudioEventData : public BopAudio::IATLEventData_bopaudio
{
public:
    MOCK_CONST_METHOD0(GetAtlEventId, Audio::TAudioEventID()){};
};
