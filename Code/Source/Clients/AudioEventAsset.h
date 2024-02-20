#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "Engine/Id.h"

namespace BopAudio
{
    class AudioEventAsset : AZ::Data::AssetData
    {
    public:
        AZ_DISABLE_COPY_MOVE(AudioEventAsset);

        AudioEventAsset() = default;
        ~AudioEventAsset() override = default;

        [[nodiscard]] auto GetId() const -> AudioEventId
        {
            return m_id;
        }

        [[nodiscard]] auto IsResource(ResourceRef const& resourceId) const -> bool
        {
            return m_id.IsValid() && (m_id.ToName() == resourceId.ToName());
        }

    private:
        AudioEventId m_id{};
    };
} // namespace BopAudio
