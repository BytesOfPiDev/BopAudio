#pragma once

#include "AzCore/EBus/EBus.h"
#include "AzCore/std/hash.h"
#include "Engine/Id.h"

namespace BopAudio
{
    struct StartEventData;
    struct StopEventData;
    class AudioObject;

    struct AudioEventBusIdType
    {
        AZ_TYPE_INFO_WITH_NAME(
            AudioEventBusIdType,
            "EventNotificationIdType",
            "{F24D8AA5-0751-4129-B930-E2985D1E0B2B}");

        AudioEventBusIdType() = default;

        AudioEventBusIdType(AudioEventId eventId)
            : m_eventId{ eventId }
        {
        }

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            constexpr AZStd::hash<AudioEventId> controlHasher;

            return controlHasher(m_eventId);
        }

        [[nodiscard]] constexpr auto operator==(AudioEventBusIdType const& rhs) const -> bool
        {
            auto const sameControl{ m_eventId == rhs.m_eventId };
            return sameControl;
        }

        [[nodiscard]] constexpr auto operator!=(AudioEventBusIdType const& rhs) const -> bool
        {
            return !(*this == rhs);
        }

        AudioEventId m_eventId;
    };

    static_assert(AZStd::is_pod_v<AudioEventBusIdType>, "Must be POD");

    class AudioEventRequests
    {
    public:
        AZ_DISABLE_COPY_MOVE(AudioEventRequests);

        AudioEventRequests() = default;
        virtual ~AudioEventRequests() = default;

        virtual void StartAudioEvent(StartEventData) = 0;
    };

    struct AudioEventRequestBusTraits : public AZ::EBusTraits
    {
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static bool const EnableEventQueue = false;
        using MutexType = AZStd::recursive_mutex;
        using BusIdType = AudioEventBusIdType;
    };

    using AudioEventRequestBus = AZ::EBus<AudioEventRequests, AudioEventRequestBusTraits>;
} // namespace BopAudio
