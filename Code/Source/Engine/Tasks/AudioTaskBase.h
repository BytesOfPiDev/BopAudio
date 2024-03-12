#pragma once

#include "AzCore/Preprocessor/Enum.h"

#include "Engine/Id.h"

namespace BopAudio
{
    AZ_ENUM_CLASS(TaskState, Invalid, Executing, Finished, Error);
    class AudioObject;

    template<typename DerivedTask>
    struct AudioTaskBase
    {
        AudioObjectId m_targetObject;
        TaskState m_state;

        [[nodiscard]] auto TryStart(AudioObject& obj) const -> bool
        {
            auto const& self{ static_cast<DerivedTask const&>(*this) };
            self(obj);

            return true;
        }

        [[nodiscard]] constexpr auto TryCancel(AudioObject&) const -> bool
        {
            return true;
        }

        [[nodiscard]] constexpr auto TryCancel() const -> bool
        {
            return true;
        }
    };

} // namespace BopAudio
