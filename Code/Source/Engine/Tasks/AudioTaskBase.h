#pragma once

#include <AzCore/RTTI/RTTI.h>

#include "AzCore/Memory/SystemAllocator.h"

namespace AZ
{
    class ReflectContext;
}

namespace BopAudio
{
    AZ_ENUM_CLASS(TaskState, Invalid, Executing, Finished, Error);
    class AudioObject;

    class IAudioTask
    {
    public:
        AZ_DEFAULT_COPY_MOVE(IAudioTask);
        AZ_CLASS_ALLOCATOR(IAudioTask, AZ::SystemAllocator);
        AZ_RTTI(IAudioTask, "{10B1622F-C67B-49C7-9D78-E0C729F15A6E}");

        IAudioTask() = default;
        virtual ~IAudioTask() = default;

        virtual void Reset() = 0;
        virtual void operator()(AudioObject&) const
        {
        }

        [[nodiscard]] virtual auto TryStart(AudioObject& obj) const -> bool
        {
            auto const& self{ *this };
            self(obj);

            return true;
        }

        [[nodiscard]] virtual auto TryCancel(AudioObject&) const -> bool
        {
            return true;
        }

        [[nodiscard]] virtual auto TryCancel() const -> bool
        {
            return true;
        }

        [[nodiscard]] virtual AZStd::vector<AZ::Data::AssetId> GetDependencies() const
        {
            return {};
        }
    };
} // namespace BopAudio
