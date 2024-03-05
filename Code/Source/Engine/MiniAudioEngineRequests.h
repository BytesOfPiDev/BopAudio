#pragma once

#include "IAudioInterfacesCommonData.h"

#include "Clients/AudioEventAsset.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct StartEventData
    {
        AZ::Data::Asset<AudioEventAsset> m_eventAsset{};
        void* m_owner{};
        Audio::TAudioControlID m_audioControlId{};
        AudioEventId m_audioEventId{};
        AudioObjectId m_audioObjectId{};
    };
} // namespace BopAudio
