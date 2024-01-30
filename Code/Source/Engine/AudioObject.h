#pragma once

#include "Engine/ATLEntities_BopAudio.h"

namespace BopAudio
{

    class SoundObject
    {
    public:
        SoundObject(AZStd::string_view objectName)
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
    struct hash<BopAudio::SoundObject>
    {
        inline auto operator()(BopAudio::SoundObject const& soundObject) const -> size_t
        {
            return soundObject.GetUniqueId();
        }
    };
} // namespace AZStd
