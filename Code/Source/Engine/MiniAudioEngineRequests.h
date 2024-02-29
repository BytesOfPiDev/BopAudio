#pragma once

#include "Clients/AudioEventAsset.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    struct ActivateTriggerRequest
    {
        AZ::Data::Asset<AudioEventAsset> m_eventAsset{};
        void* m_owner{};
        // The ATL control that is activating the event.
        Audio::TAudioControlID m_audioControlId{};
    };
} // namespace BopAudio
