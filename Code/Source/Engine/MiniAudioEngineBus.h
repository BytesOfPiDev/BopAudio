#pragma once

#include <AzCore/JSON/document.h>

#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/RTTIMacros.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
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

    struct StartEventData
    {
        AZ::Data::Asset<AudioEventAsset> m_eventAsset{};
        void* m_owner{};
        Audio::TAudioControlID m_audioControlId{};
        AudioEventId m_audioEventId{};
        AudioObjectId m_audioObjectId{};
    };

    class SoundEngineRequests
    {
    public:
        AZ_RTTI(SoundEngineRequests, "{CBAFA763-278C-4D48-9CE3-E11B78492F72}");
        AZ_DISABLE_COPY_MOVE(SoundEngineRequests);

        SoundEngineRequests() = default;
        virtual ~SoundEngineRequests() = default;

        [[nodiscard]] virtual auto Initialize() -> NullOutcome = 0;
        [[nodiscard]] virtual auto Shutdown() -> NullOutcome = 0;

        [[nodiscard]] virtual auto RegisterFile(RegisterFileData const&) -> NullOutcome = 0;
        [[nodiscard]] virtual auto CreateAudioObject() -> AudioObjectId = 0;
        virtual void RemoveAudioObject(AudioObjectId) = 0;
        [[nodiscard]] virtual auto StartEvent(StartEventData const&) -> NullOutcome = 0;
        virtual auto StopEvent(AudioEventId eventId) -> bool = 0;

        [[nodiscard]] virtual auto GetSoundEngine() -> ma_engine* = 0;
        [[nodiscard]] virtual auto LoadSound(SoundRef const& resourceRef) -> NullOutcome = 0;
    };

    using SoundEngine = AZ::Interface<SoundEngineRequests>;
} // namespace BopAudio
