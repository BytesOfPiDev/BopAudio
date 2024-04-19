#pragma once

#include "Engine/Id.h"
#include <AzCore/EBus/EBus.h>
#include <IAudioInterfacesCommonData.h>

namespace BopAudio
{

    class AudioObject;

    class MiniAudioEventRequests
    {
    public:
        AZ_DEFAULT_COPY_MOVE(MiniAudioEventRequests);
        MiniAudioEventRequests() = default;
        virtual ~MiniAudioEventRequests() = default;
        virtual auto TryStartEvent(AudioObject& /*audioObject*/) -> bool = 0;
        virtual auto TryStopEvent(AudioObject& /*audioObject*/) -> bool = 0;
        [[nodiscard]] virtual auto GetEventState() const -> Audio::EAudioEventState = 0;
    };

    struct MiniAudioEventRequestBusTraits : public AZ::EBusTraits
    {
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AudioEventId;
    };

    using MiniAudioEventRequestBus =
        AZ::EBus<MiniAudioEventRequests, MiniAudioEventRequestBusTraits>;
} // namespace BopAudio
