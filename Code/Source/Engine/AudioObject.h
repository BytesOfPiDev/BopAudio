#pragma once

#include "AzCore/Math/Transform.h"
#include "Engine/AudioEvent.h"
#include "Engine/Id.h"

namespace BopAudio
{
    class AudioEvent;

    struct AudioObject
    {
        AZ_DISABLE_COPY(AudioObject);

        AudioObject() = default;
        AudioObject(AZStd::string_view objectName)
            : m_name{ objectName } {};
        ~AudioObject() = default;

        constexpr auto operator==(InstanceId instanceId) -> bool
        {
            return m_instanceId == instanceId;
        }

        constexpr auto operator==(AudioObjectId instanceId) -> bool
        {
            return m_audioObjectId == instanceId;
        }

        constexpr auto operator!=(InstanceId instanceId) -> bool
        {
            return !(m_instanceId == instanceId);
        }

        constexpr auto operator!=(AudioObjectId audioObjectId) -> bool
        {
            return !(m_audioObjectId == audioObjectId);
        }

        void Update(float deltaTime);

        [[nodiscard]] auto GetId() const -> AudioObjectId
        {
            return m_audioObjectId;
        }

        void AddEvent(AudioEvent const& audioEvent);

    private:
        InstanceId m_instanceId{};
        AudioObjectId m_audioObjectId{};
        AZ::Name m_name{};
        AZStd::vector<AudioEvent> m_events{};
        [[maybe_unused]] AZ::Transform m_transform{};
    };

}; // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::AudioObject>
    {
        inline auto operator()(BopAudio::AudioObject const& soundObject) const -> size_t
        {
            return soundObject.GetId().GetHash();
        }
    };
} // namespace AZStd
