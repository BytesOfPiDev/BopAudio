#include <AzTest/AzTest.h>

#include "Engine/ATLEntities_BopAudio.h"
#include "IAudioInterfacesCommonData.h"

class MockAudioObjectData : public BopAudio::IATLAudioObjectData_bopaudio
{
public:
    MOCK_CONST_METHOD0(GetAtlAudioObjectId, Audio::TAudioObjectID());
    MOCK_METHOD1(ChangeName, void(AZStd::string_view));
    MOCK_CONST_METHOD0(GetName, AZ::Name());
};
