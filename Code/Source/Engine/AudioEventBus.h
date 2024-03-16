#pragma once

#include "AzCore/EBus/EBus.h"
#include "AzCore/std/hash.h"
#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    struct EventStartedData;
    struct EventFinishedData;
    class AudioObject;

    AZ_ENUM_CLASS(AudioEventState, Idle, Active);

    struct EventNotificationIdType
    {
        AZ_TYPE_INFO_WITH_NAME(
            EventNotificationIdType,
            "EventNotificationIdType",
            "{F24D8AA5-0751-4129-B930-E2985D1E0B2B}");

        EventNotificationIdType() = default;

        EventNotificationIdType(Audio::TAudioControlID controlId, void* owner)
            : m_atlControlId{ controlId }
            , m_owner(reinterpret_cast<uintptr_t>(owner))
        {
        }
        EventNotificationIdType(Audio::TAudioControlID controlId, AZ::EntityId owner)
            : m_atlControlId{ controlId }
            , m_owner(static_cast<uintptr_t>(static_cast<AZ::u64>(owner)))
        {
        }

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            constexpr AZStd::hash<uintptr_t> ownerHasher;
            constexpr AZStd::hash<Audio::TAudioControlID> controlHasher;

            size_t hash{};
            AZStd::hash_combine(hash, ownerHasher(m_owner), controlHasher(m_atlControlId));
            return hash;
        }

        [[nodiscard]] constexpr auto operator==(EventNotificationIdType const& rhs) const -> bool
        {
            return (m_owner == rhs.m_owner) && m_atlControlId == rhs.m_atlControlId;
        }

        [[nodiscard]] constexpr auto operator!=(EventNotificationIdType const& rhs) const -> bool
        {
            return !(*this == rhs);
        }

        uintptr_t m_owner;
        Audio::TAudioControlID m_atlControlId;
    };

    static_assert(AZStd::is_pod_v<EventNotificationIdType>, "Must be POD");

    class MiniAudioEventRequests
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEventRequests);
        AZ_RTTI(MiniAudioEventRequests, "{4E60C34B-EBD3-47BE-87CA-42E4500BCD29}");

        MiniAudioEventRequests() = default;
        virtual ~MiniAudioEventRequests() = default;

        [[nodiscard]] virtual auto TryStartEvent(AudioObject&) -> bool = 0;
        [[nodiscard]] virtual auto TryStopEvent(AudioObject&) -> bool = 0;

        [[nodiscard]] virtual auto GetEventState() const -> AudioEventState = 0;
    };

    struct AudioEventRequestBusTraits : AZ::EBusTraits
    {
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        using MutexType = AZStd::recursive_mutex;
        using BusIdType = AudioEventId;
    };

    using MiniAudioEventRequestBus = AZ::EBus<MiniAudioEventRequests, AudioEventRequestBusTraits>;

    class MiniAudioEventNotifications
    {
    public:
        AZ_DISABLE_COPY_MOVE(MiniAudioEventNotifications);

        MiniAudioEventNotifications() = default;
        virtual ~MiniAudioEventNotifications() = default;

        virtual void ReportDurationInfo(
            [[maybe_unused]] Audio::TAudioControlID triggerId,
            [[maybe_unused]] Audio::TAudioEventID eventId,
            [[maybe_unused]] float duration,
            [[maybe_unused]] float estimatedDuration) = 0;
        virtual void ReportEventStarted(EventStartedData const&) = 0;
        virtual void ReportEventFinished(EventFinishedData const&) = 0;
        virtual void ReportPreTryStart() = delete;
    };

    struct AudioEventNotificationBusTraits : public AZ::EBusTraits
    {
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static bool const EnableEventQueue = true;
        using MutexType = AZStd::recursive_mutex;
        using BusIdType = EventNotificationIdType;
    };

    using AudioEventNotificationBus =
        AZ::EBus<MiniAudioEventNotifications, AudioEventNotificationBusTraits>;
} // namespace BopAudio
