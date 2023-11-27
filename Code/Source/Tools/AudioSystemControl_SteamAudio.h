#pragma once

#include "ACETypes.h"
#include "AzCore/Preprocessor/Enum.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemControl.h"

namespace SteamAudio
{
    AZ_ENUM_WITH_UNDERLYING_TYPE( // NOLINT
        SteamAudioControlType,
        AudioControls::TImplControlType,
        (Invalid, 0),
        (Trigger, AUDIO_BIT(0)),
        (Rtpc, AUDIO_BIT(1)),
        (Switch, AUDIO_BIT(2)),
        (AuxBus, AUDIO_BIT(3)),
        (SoundLibrary, AUDIO_BIT(4)),
        (GameState, AUDIO_BIT(5)),
        (SwitchGroup, AUDIO_BIT(6)),
        (GameStateGroup, AUDIO_BIT(7)),
        (Environment, AUDIO_BIT(8)));

    class IAudioSystemControl_SteamAudio : public AudioControls::IAudioSystemControl // NOLINT
    {
    public:
        IAudioSystemControl_SteamAudio() = default;
        IAudioSystemControl_SteamAudio(const AZStd::string& name, AudioControls::CID id, AudioControls::TImplControlType type);
        ~IAudioSystemControl_SteamAudio() override = default;
    };
} // namespace SteamAudio
