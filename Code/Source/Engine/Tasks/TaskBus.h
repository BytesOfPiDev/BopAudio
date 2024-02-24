#pragma once

#include "AzCore/EBus/EBus.h"
#include <AzCore/std/any.h>

namespace BopAudio
{
    class TaskFactoryRequests
    {
    public:
        [[nodiscard]] virtual auto Create(AZStd::span<char const> dataBuffer) const
            -> AZStd::any = 0;
    };

    class TaskFactoryRequestBusTraits : public AZ::EBusTraits
    {
    public:
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;

        using BusIdType = AZ::Crc32;
    };

    using TaskFactoryBus = AZ::EBus<TaskFactoryRequests, TaskFactoryRequestBusTraits>;
} // namespace BopAudio
