#pragma once

#include "Engine/Id.h"

namespace BopAudio
{
    struct ActivateTriggerRequest
    {
        //! The trigger to activate on an audio object.
        NamedResource m_triggerResource;
        //! The object to activate the trigger on.
        AudioObjectId m_audioObjectId;
        //! An ID to associate with the trigger->event.
        AZ::u32 m_eventId{};
    };
} // namespace BopAudio
