#pragma once

#include "AzCore/Preprocessor/Enum.h"

#include "Engine/Id.h"

namespace BopAudio
{
    AZ_ENUM_CLASS(TaskState, Invalid, Executing, Finished, Error);

    template<typename DerivedTask>
    struct AudioTaskBase
    {
        AudioObjectId m_targetObject;
        TaskState m_state;
    };

} // namespace BopAudio
