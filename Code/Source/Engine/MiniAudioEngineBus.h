#pragma once

#include <AzCore/JSON/document.h>

#include "ATLEntityData.h"
#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/RTTIMacros.h"

#include "BopAudio/Util.h"
#include "Engine/Id.h"
#include "IAudioInterfacesCommonData.h"

struct ma_engine;

namespace BopAudio
{
    struct ActivateTriggerRequest;

    class AudioEngineRequests
    {
    public:
        AZ_RTTI(AudioEngineRequests, "{CBAFA763-278C-4D48-9CE3-E11B78492F72}");
        AZ_DISABLE_COPY_MOVE(AudioEngineRequests);

        AudioEngineRequests() = default;
        virtual ~AudioEngineRequests() = default;

        [[nodiscard]] virtual auto Initialize() -> NullOutcome = 0;
        virtual auto Shutdown() -> bool = 0;

        [[nodiscard]] virtual auto LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
            -> NullOutcome = 0;
        [[nodiscard]] virtual auto CreateAudioObject(Audio::TAudioObjectID audioObjectId)
            -> bool = 0;
        virtual void RemoveAudioObject(Audio::TAudioObjectID audioObjectId) = 0;
        virtual auto ActivateTrigger(ActivateTriggerRequest const&) -> AudioOutcome<void> = 0;

        virtual auto GetSoundEngine() -> ma_engine* = 0;
    };

    using AudioEngineInterface = AZ::Interface<AudioEngineRequests>;
} // namespace BopAudio
