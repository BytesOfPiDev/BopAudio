#include "Engine/AudioObject.h"

#include "AzCore/std/containers/vector.h"

namespace BopAudio
{

    void AudioObject::Update(float)
    {
        for (AudioEvent& event : m_events)
        {
            event.Execute();
        };
    }

    void AudioObject::AddEvent(AudioEvent const& audioEvent)
    {
        m_events.push_back(audioEvent);
    }
} // namespace BopAudio
