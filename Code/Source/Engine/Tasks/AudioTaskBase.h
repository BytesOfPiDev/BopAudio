#pragma once

#include <AzCore/RTTI/RTTI.h>

namespace BopAudio
{
    AZ_ENUM_CLASS(TaskState, Invalid, Executing, Finished, Error);
    class AudioObject;

    class IAudioTask
    {
    public:
        AZ_DEFAULT_COPY_MOVE(IAudioTask);
        AZ_RTTI(IAudioTask, "{10B1622F-C67B-49C7-9D78-E0C729F15A6E}");

        IAudioTask() = default;
        virtual ~IAudioTask() = default;

        virtual void Reset() = 0;
    };

    template<typename DerivedTask>
    struct AudioTaskBase : public IAudioTask
    {
        void Reset() override
        {
        }

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
