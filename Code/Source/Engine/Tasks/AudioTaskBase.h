#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include "AzCore/Memory/SystemAllocator.h"

namespace AZ
{
    class ReflectContext;
}

namespace BopAudio
{
    AZ_ENUM_CLASS(TaskState, Invalid, Executing, Finished, Error);
    class AudioObject;
    class AudioTaskPtr;

    class IAudioTask
    {
        friend AudioTaskPtr;

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

        [[nodiscard]] virtual auto GetDependencies() const -> AZStd::vector<AZ::Data::AssetId>
        {
            return {};
        }

    private:
        virtual auto CloneAudioTask() -> AZStd::unique_ptr<IAudioTask>
        {
            return {};
        }
    };

    class AudioTaskPtr final
    {
    public:
        AZ_TYPE_INFO(AudioTaskPtr, "{CA2C3D89-5E2B-4CAA-86B1-3F0D4380978C}");

        AudioTaskPtr() = default;
        AudioTaskPtr(AudioTaskPtr&& other)
            : m_task(other.m_task)
        {
            other.m_task = nullptr;
        }

        AudioTaskPtr(AudioTaskPtr const& other)
            : m_task{ other.m_task->CloneAudioTask().release() } {};

        auto operator=(AudioTaskPtr const& rhs) -> AudioTaskPtr&
        {
            AZ_Verify(&rhs != this, "Illegal self-assignment.\n");
            {
                m_task = rhs.m_task->CloneAudioTask().release();
            }

            return *this;
        }

        auto operator=(AudioTaskPtr&& rhs) -> AudioTaskPtr&
        {
            AZ_Verify(&rhs != this, "Illegal self-assignment.\n");

            m_task = rhs.m_task;
            rhs.m_task = nullptr;

            return *this;
        }

        ~AudioTaskPtr()
        {
            delete m_task;
        }

        void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<AudioTaskPtr>()->Version(0);

                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<AudioTaskPtr>("AudioTaskPtr", "")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
                }
            }
        }

    private:
        IAudioTask* m_task{};
    };
} // namespace BopAudio
