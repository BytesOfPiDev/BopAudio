#pragma once

#include "ATLEntityData.h"
#include "AzCore/Interface/Interface.h"
#include "AzCore/JSON/document.h"
#include "AzCore/RTTI/RTTIMacros.h"
#include "Engine/Id.h"

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
        [[nodiscard]] virtual auto CreateAudioObject(UniqueId const&) -> bool = 0;
        virtual void RemoveAudioObject(UniqueId audioObjectId) = 0;
        virtual auto ActivateTrigger(ActivateTriggerRequest const&) -> AudioEventId = 0;
        [[nodiscard]] virtual auto FindSoundBank(NamedResource const& resourceId) const
            -> AZStd::shared_ptr<rapidjson::Document> = 0;

        virtual auto GetSoundEngine() -> ma_engine* = 0;
    };

    using AudioEngineInterface = AZ::Interface<AudioEngineRequests>;
} // namespace BopAudio
