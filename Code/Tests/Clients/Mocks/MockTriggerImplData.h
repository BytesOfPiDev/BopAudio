#pragma once

#include <AzTest/AzTest.h>

#include "Engine/ATLEntities_BopAudio.h"
#include "IAudioInterfacesCommonData.h"

class MockTriggerImplData : public BopAudio::IATLTriggerImplData_bopaudio
{
public:
    MOCK_CONST_METHOD0(GetEventId, Audio::TAudioEventID());
};
