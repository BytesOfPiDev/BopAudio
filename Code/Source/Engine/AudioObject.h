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

        AudioObject() = default;
        AudioObject(AZStd::string_view objectName)
            : m_name{ objectName } {};
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
        AZ::Name m_name{};
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
