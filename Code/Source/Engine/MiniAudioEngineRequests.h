#pragma once

#include "Engine/ATLEntities_BopAudio.h"

namespace BopAudio
{
    struct ActivateTriggerRequest
    {
        //! The Trigger to activate
        BA_TriggerId m_triggerId;
        //! The object to activate the trigger on.
        BA_GameObjectId m_audioObjectId;
        AZ::Name m_soundName;

        SATLEventData_BopAudio* m_eventData;
    };
} // namespace BopAudio
