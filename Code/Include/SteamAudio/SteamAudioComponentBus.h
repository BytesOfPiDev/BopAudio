#pragma once

#include "AzCore/Component/ComponentBus.h"

namespace SteamAudio
{
    class SteamAudioComponentRequests : public AZ::ComponentBus
    {
    public:
    };

    using SteamAudioComponentRequestBus = AZ::EBus<SteamAudioComponentRequests>;

} // namespace SteamAudio
