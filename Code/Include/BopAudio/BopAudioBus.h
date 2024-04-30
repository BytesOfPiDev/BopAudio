
#pragma once

#include "AzCore/EBus/EBus.h"
#include "AzCore/Interface/Interface.h"
#include "IAudioSystemImplementation.h"

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(
        Audio::AudioSystemImplementationRequests, "{51746815-C8EE-4FA8-965B-6EE541840F0A}")
}

namespace BopAudio
{
    class SoundSource
    {
    };

    class BopAudioRequests
    {
    public:
        AZ_DISABLE_COPY_MOVE(BopAudioRequests);
        AZ_RTTI(BopAudioRequests, "{33D964A5-4B02-4D91-9DED-7FE4568E058B}");

        BopAudioRequests() = default;
        virtual ~BopAudioRequests() = default;
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
