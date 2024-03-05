#pragma once

#include <AzCore/JSON/document.h>

#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/RTTIMacros.h"

#include "BopAudio/Util.h"
#include "Engine/Id.h"

struct ma_engine;

namespace BopAudio
{
    struct StartEventData;

    struct RegisterFileData
    {
        AZStd::span<char const> m_buffer;
        AZStd::string_view m_fileName;
    };

    class AudioEngineRequests
    {
    public:
        AZ_RTTI(AudioEngineRequests, "{CBAFA763-278C-4D48-9CE3-E11B78492F72}");
        AZ_DISABLE_COPY_MOVE(AudioEngineRequests);

        AudioEngineRequests() = default;
        virtual ~AudioEngineRequests() = default;

        [[nodiscard]] virtual auto Initialize() -> NullOutcome = 0;
        [[nodiscard]] virtual auto Shutdown() -> bool = 0;

        [[nodiscard]] virtual auto RegisterFile(RegisterFileData const&) -> NullOutcome = 0;
        [[nodiscard]] virtual auto CreateAudioObject() -> AudioObjectId = 0;
        virtual void RemoveAudioObject(AudioObjectId) = 0;
        [[nodiscard]] virtual auto StartEvent(StartEventData const&) -> NullOutcome = 0;
        virtual auto StopEvent(AudioEventId eventId) -> bool = 0;

        [[nodiscard]] virtual auto GetSoundEngine() -> ma_engine* = 0;
        [[nodiscard]] virtual auto LoadSound(SoundRef const& resourceRef) -> NullOutcome = 0;
    };

    using AudioEngineInterface = AZ::Interface<AudioEngineRequests>;
} // namespace BopAudio
