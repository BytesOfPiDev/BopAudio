#pragma once

#include "Engine/ATLEntities_BopAudio.h"

namespace BopAudio
{

    class AudioObject
    {
    public:
        AudioObject(AZStd::string_view objectName)
            : m_uniqueId{ objectName } {};

        [[nodiscard]] auto GetUniqueId() const -> BA_UniqueId
        {
            return m_uniqueId;
        }

    private:
        BA_UniqueId m_uniqueId;
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
