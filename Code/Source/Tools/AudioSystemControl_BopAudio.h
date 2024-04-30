#pragma once

#include "ACETypes.h"
#include "AzCore/Preprocessor/Enum.h"
#include "AzCore/base.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemControl.h"

namespace BopAudio
{
    AZ_ENUM_WITH_UNDERLYING_TYPE(
        BopAudioControlType,
        AudioControls::TImplControlType,
        (Invalid, 0),
        (Event, AUDIO_BIT(0)),
        (Rtpc, AUDIO_BIT(1)),
        (Switch, AUDIO_BIT(2)),
        (AuxBus, AUDIO_BIT(3)),
        (SoundBank, AUDIO_BIT(4)),
        (GameState, AUDIO_BIT(5)),
        (SwitchGroup, AUDIO_BIT(6)),
        (GameStateGroup, AUDIO_BIT(7)),
        (Environment, AUDIO_BIT(8)));

    class IAudioSystemControl_bopaudio : public AudioControls::IAudioSystemControl
    {
    public:
        AZ_DEFAULT_COPY_MOVE(IAudioSystemControl_bopaudio)

        IAudioSystemControl_bopaudio() = default;
        IAudioSystemControl_bopaudio(
            AZStd::string const& name, AudioControls::CID id, AudioControls::TImplControlType type);
        ~IAudioSystemControl_bopaudio() override = default;
    };
} // namespace BopAudio
