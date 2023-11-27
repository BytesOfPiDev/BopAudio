#include "AudioSystemControl_SteamAudio.h"
#include <ACETypes.h>
#include <IAudioSystemControl.h>

namespace SteamAudio
{
    IAudioSystemControl_SteamAudio::IAudioSystemControl_SteamAudio(
        AZStd::string const& name, AudioControls::CID id, AudioControls::TImplControlType type)
        : IAudioSystemControl(name, id, type)
    {
    }
} // namespace SteamAudio
