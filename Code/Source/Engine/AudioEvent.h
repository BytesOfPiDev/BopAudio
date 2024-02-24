#pragma once

#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"
#include <AzCore/Memory/Memory_fwd.h>
#include <AzCore/RTTI/TypeInfoSimple.h>
namespace BopAudio
{
    static constexpr auto MaxTasks{ 3 };

    class AudioEvent
    {
    public:
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEvent);
        AZ_CLASS_ALLOCATOR_DECL;
        static void Reflect(AZ::ReflectContext* context);

        /*
         * The reference/id/name of this event.
         *
         * NOTE: This is what you use to instantiate a new event that matches how this instance was
         * created.
         */
        AudioEventId m_id{};

        /*
         * Stores implementation data that implements the event.
         *
         * It is type-erased to avoid putting the, potentially many, template/variant types in the
         * header, allowing the implementation to be be held entirely in the source file. A PIMPL
         * pattern may be better, which can be evaluated at a later time.
         */
        AZStd::any m_internalData{};

        /*
         * The current state of the event.
         *
         * It is not serialized, as it is only meaningful at run-time.
         */
        Audio::EAudioEventState m_eventState{};
    };
} // namespace BopAudio
