#pragma once

#include "Engine/ATLEntities_BopAudio.h"

namespace BopAudio
{

    struct AudioObject
    {
        AZ_DEFAULT_COPY_MOVE(AudioObject);

        AudioObject() = default;
        AudioObject(AZStd::string_view objectName)
            : m_id{ objectName } {};
        ~AudioObject() = default;

        [[nodiscard]] auto GetUniqueId() const -> BA_UniqueId
        {
            return m_id;
        }

        BA_UniqueId m_id{};
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
            return soundObject.GetUniqueId();
        }
    };
} // namespace AZStd
