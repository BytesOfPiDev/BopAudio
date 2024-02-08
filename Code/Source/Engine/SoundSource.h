#pragma once

#include "AzCore/IO/Path/Path_fwd.h"
#include "AzCore/Name/Name.h"
#include "Engine/Id.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    class SoundSource
    {
    public:
        SoundSource() = default;
        SoundSource(AZ::IO::Path localPath);

        [[nodiscard]] auto GetAsset() const -> MiniAudio::SoundDataAsset
        {
            return m_soundAsset;
        }

        auto Load() -> AZ::Outcome<void, char const*>;

        constexpr auto IsReady() -> bool
        {
            return m_soundAsset.IsReady();
        }

        [[nodiscard]] auto GetResourceId() const -> NamedResource
        {
            return NamedResource{ m_name };
        };

    private:
        AZ::Name m_name;
        MiniAudio::SoundDataAsset m_soundAsset;
    };
} // namespace BopAudio
