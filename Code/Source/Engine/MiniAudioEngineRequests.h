#pragma once

#include "Engine/ATLEntities_BopAudio.h"

namespace BopAudio
{
    struct ActivateTriggerRequest
    {
        //! The Trigger to activate
        // BA_TriggerId m_triggerId;
        //! The object to activate the trigger on.
        UniqueId m_objectInstanceId;
        ResourceId m_triggerResource;
    };
} // namespace BopAudio
