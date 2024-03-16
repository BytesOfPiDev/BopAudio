#pragma once

#include "AzCore/Memory/Memory_fwd.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "IAudioInterfacesCommonData.h"

#include "Clients/AudioEventAsset.h"
#include "Engine/Id.h"

namespace BopAudio
{
    class AudioEvent
    {
    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DEFAULT_COPY_MOVE(AudioEvent);
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEvent);

        static void Reflect(AZ::ReflectContext* context);

        AudioEvent() = default;
        AudioEvent(AZ::Data::Asset<AudioEventAsset> eventAsset);
        ~AudioEvent() = default;

    protected:
        [[nodiscard]] constexpr auto operator==(AudioEvent const& otherEvent) const
        {
            return m_id == otherEvent.m_id;
        }

        [[nodiscard]] constexpr auto operator!=(AudioEvent const& otherEvent) const
        {
            return !((*this) == otherEvent);
        }

        [[nodiscard]] constexpr auto operator<(AudioEvent const& otherEvent) const
        {
            return m_id < otherEvent.m_id;
        }

        [[nodiscard]] constexpr auto operator>(AudioEvent const& otherEvent) const
        {
            return m_id > otherEvent.m_id;
        }

    private:
        /*
         * The reference/id/name of this event.
         *
         * NOTE: This is what you use to instantiate a new event that matches how this instance was
         * created.
         */
        [[maybe_unused]] AudioEventId m_id{};

        /*
         * Stores implementation data that implements the event.
         *
         * It is type-erased to avoid putting the, potentially many, template/variant types in the
         * header, allowing the implementation to be be held entirely in the source file. A PIMPL
         * pattern may be better, which can be evaluated at a later time.
         */
        [[maybe_unused]] AZStd::any m_internalData{};

        /*
         * The current state of the event.
         *
         * It is not serialized, as it is only meaningful at run-time.
         */
        [[maybe_unused]] Audio::EAudioEventState m_eventState{};

        AZ::Data::Asset<AudioEventAsset> m_eventAsset{};
    };
} // namespace BopAudio
