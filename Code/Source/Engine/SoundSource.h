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
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DEFAULT_COPY_MOVE(SoundSource);
        AZ_TYPE_INFO_WITH_NAME_DECL(SoundSource);

        SoundSource() = default;
        ~SoundSource() = default;
        SoundSource(AZ::IO::Path localPath);

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetAsset() const -> MiniAudio::SoundDataAsset
        {
            return m_soundAsset;
        }

        auto Load() -> AZ::Outcome<void, char const*>;

        constexpr auto IsReady() -> bool
        {
            return m_soundAsset.IsReady();
        }

        [[nodiscard]] auto GetResourceId() const -> ResourceRef
        {
            return ResourceRef{ m_name };
        };

    private:
        AZ::Name m_name;
        MiniAudio::SoundDataAsset m_soundAsset;
    };
} // namespace BopAudio
