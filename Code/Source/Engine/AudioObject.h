#pragma once

#include "AzCore/Math/Transform.h"
#include "Engine/Id.h"

namespace BopAudio
{

    struct AudioObject
    {
        AZ_DEFAULT_COPY(AudioObject);
        AZ_DISABLE_MOVE(AudioObject);

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

        [[nodiscard]] auto GetId() const -> AudioObjectId
        {
            return m_audioObjectId;
        }

    private:
        InstanceId m_instanceId{};
        AudioObjectId m_audioObjectId{};
        AZ::Name m_name{};
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
