#include "Engine/AudioObject.h"

#include "AzCore/std/containers/vector.h"

namespace BopAudio
{

    void AudioObject::Update(float)
    {
        for (AudioEvent& event : m_events)
        {
            event(*this);
        };
    }

    void AudioObject::AddEvent(AudioEvent const& audioEvent)
    {
        m_events.push_back(audioEvent);
    }

    auto AudioObject::AddEvent([[maybe_unused]] AudioEventId eventResourceId)
        -> AudioOutcome<AudioEvent>
    {
        AZ_Error("AudioObject", false, "Not implemented.");
        return AZ::Failure("Not implemented.");
    }
} // namespace BopAudio
