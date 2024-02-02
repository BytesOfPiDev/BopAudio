#pragma once

#include "ATLEntityData.h"
#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/RTTIMacros.h"
#include "Engine/ATLEntities_BopAudio.h"

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

        virtual auto Initialize() -> bool = 0;
        virtual auto Shutdown() -> bool = 0;

        virtual auto LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool = 0;
        [[nodiscard]] virtual auto CreateAudioObject(SATLAudioObjectData_BopAudio* const audioObjectData = nullptr) -> BA_GameObjectId = 0;
        virtual void RemoveAudioObject(BA_UniqueId audioObjectId) = 0;
        virtual auto ActivateTrigger(ActivateTriggerRequest const&) -> bool = 0;

        virtual auto GetSoundEngine() -> ma_engine* = 0;
    };

    using AudioEngineInterface = AZ::Interface<AudioEngineRequests>;
} // namespace BopAudio
