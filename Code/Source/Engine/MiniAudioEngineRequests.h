#pragma once

#include "Engine/ATLEntities_BopAudio.h"
namespace BopAudio
{
    struct ActivateTriggerRequest
    {
        //! The Trigger to activate
        BA_TriggerId m_triggerId;
        //! The object to activate the trigger on.
        BA_UniqueId m_audioObjectId;
    };
} // namespace BopAudio
