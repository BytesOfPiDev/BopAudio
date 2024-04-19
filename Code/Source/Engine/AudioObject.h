#pragma once

#include "Clients/AudioEventAsset.h"
#include "Engine/Id.h"

namespace BopAudio
{
    class AudioEventAsset;

    class AudioObject
    {
    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY(AudioObject);
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioObject);

        AudioObject();
        ~AudioObject() = default;

        constexpr auto operator==(AudioObjectId instanceId) -> bool
        {
            return m_audioObjectId == instanceId;
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

    private:
        AudioObjectId m_audioObjectId{};
    };

}; // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::AudioObject>
    {
        inline auto operator()(BopAudio::AudioObject const& soundObject) const -> size_t
        {
            return static_cast<size_t>(soundObject.GetId());
        }
    };
} // namespace AZStd
