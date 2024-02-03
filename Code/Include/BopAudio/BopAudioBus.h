
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace BopAudio
{
    class BopAudioRequests
    {
    public:
        AZ_RTTI(BopAudioRequests, "{33D964A5-4B02-4D91-9DED-7FE4568E058B}");
        virtual ~BopAudioRequests() = default;
        // Put your public methods here
    };

    class BopAudioBusTraits : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using BopAudioRequestBus = AZ::EBus<BopAudioRequests, BopAudioBusTraits>;
    using BopAudioInterface = AZ::Interface<BopAudioRequests>;

} // namespace BopAudio
