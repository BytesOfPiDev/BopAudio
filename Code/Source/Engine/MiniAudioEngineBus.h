#pragma once

#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/RTTIMacros.h"

struct ma_engine;

namespace BopAudio
{
    class AudioEngineRequests
    {
    public:
        AZ_RTTI(AudioEngineRequests, "{CBAFA763-278C-4D48-9CE3-E11B78492F72}");
        AZ_DISABLE_COPY_MOVE(AudioEngineRequests);

        AudioEngineRequests() = default;
        virtual ~AudioEngineRequests() = default;

        virtual auto GetSoundEngine() -> ma_engine* = 0;
    };

    using AudioEngineInterface = AZ::Interface<AudioEngineRequests>;
} // namespace BopAudio
