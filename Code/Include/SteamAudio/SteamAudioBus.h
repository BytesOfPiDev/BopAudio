
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace SteamAudio
{
    class SteamAudioRequests
    {
    public:
        AZ_RTTI(SteamAudioRequests, "{33D964A5-4B02-4D91-9DED-7FE4568E058B}");
        virtual ~SteamAudioRequests() = default;
        // Put your public methods here
    };
    
    class SteamAudioBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using SteamAudioRequestBus = AZ::EBus<SteamAudioRequests, SteamAudioBusTraits>;
    using SteamAudioInterface = AZ::Interface<SteamAudioRequests>;

} // namespace SteamAudio
