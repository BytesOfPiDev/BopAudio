#pragma once

#include "AzCore/EBus/EBus.h"
#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    struct StartEventData;
    struct StopEventData;
    class AudioObject;

    class AudioEventBusIdType
    {
    public:
        AZ_TYPE_INFO_WITH_NAME(
            AudioEventBusIdType,
            "EventNotificationIdType",
            "{F24D8AA5-0751-4129-B930-E2985D1E0B2B}");

        AudioEventBusIdType() = default;

        AudioEventBusIdType(AudioEventId eventId)
            : m_eventId{ eventId } {};

        [[nodiscard]] constexpr auto operator==(AudioEventBusIdType const& rhs) const -> bool
        {
            auto const sameControl{ m_eventId == rhs.m_eventId };
            return sameControl;
        }

        [[nodiscard]] constexpr auto operator!=(AudioEventBusIdType const& rhs) const -> bool
        {
            return !(*this == rhs);
        }

        [[nodiscard]] explicit constexpr operator Audio::TATLIDType() const
        {
            return static_cast<Audio::TATLIDType>(m_eventId);
        }

        constexpr auto operator<(AudioEventBusIdType const&) const -> bool = delete;
        constexpr auto operator>(AudioEventBusIdType const&) const -> bool = delete;

    private:
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
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static bool const EnableEventQueue = false;
        using MutexType = AZStd::recursive_mutex;
        using BusIdType = AudioEventBusIdType;
    };

    using AudioEventRequestBus = AZ::EBus<AudioEventRequests, AudioEventRequestBusTraits>;
} // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::AudioEventBusIdType>
    {
        [[nodiscard]] auto operator()(BopAudio::AudioEventBusIdType const& busId) const -> size_t
        {
            static constexpr auto hasher{ AZStd::hash<Audio::TATLIDType>{} };
            return hasher(static_cast<Audio::TATLIDType>(busId));
        }
    };
} // namespace AZStd