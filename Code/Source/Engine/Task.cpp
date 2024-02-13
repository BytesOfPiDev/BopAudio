#include "Engine/Task.h"

#include "Engine/Events/PlaySound.h"
#include "Engine/Events/StopSound.h"

namespace BopAudio
{

    void AudioTaskHandler::operator()(PlaySoundTask const& /*playSoundData*/) const
    {
    }

    void AudioTaskHandler::operator()(StopSoundTask const& args) const
    {
    }

    void AudioTaskIsFinished::operator()(PlaySoundTask const&) const
    {
    }

    void AudioTaskIsFinished::operator()(StopSoundTask const& args) const
    {
    }

} // namespace BopAudio
