#include "AudioSystemControl_BopAudio.h"

#include "ACETypes.h"
#include "IAudioSystemControl.h"

namespace BopAudio
{
    IAudioSystemControl_bopaudio::IAudioSystemControl_bopaudio(
        AZStd::string const& name, AudioControls::CID id, AudioControls::TImplControlType type)
        : IAudioSystemControl(name, id, type){};
} // namespace BopAudio
