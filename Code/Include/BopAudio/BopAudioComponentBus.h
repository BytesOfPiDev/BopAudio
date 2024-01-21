#pragma once

#include "AzCore/Component/ComponentBus.h"

namespace BopAudio
{
    class BopAudioComponentRequests : public AZ::ComponentBus
    {
    public:
    };

    using BopAudioComponentRequestBus = AZ::EBus<BopAudioComponentRequests>;

} // namespace BopAudio
