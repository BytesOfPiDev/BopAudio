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

        SoundSource();
        ~SoundSource();
        SoundSource(ResourceRef const& soundResourceRef);
        SoundSource(AZ::IO::Path localPath);

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            return m_name.GetHash();
        }

        [[nodiscard]] constexpr auto operator==(SoundSource const& other) const
        {
            return m_name == other.m_name;
        }

        [[nodiscard]] constexpr auto operator!=(SoundSource const& other) const
        {
            return !((*this) == other);
        }

        [[nodiscard]] auto GetAsset() const -> MiniAudio::SoundDataAsset
        {
            return m_soundAsset;
        }

        auto Load() -> AZ::Outcome<void, char const*>;

        [[nodiscard]] constexpr auto IsReady() const -> bool
        {
            return m_soundAsset.IsReady();
        }

        [[nodiscard]] auto GetResourceId() const -> ResourceRef
        {
            return ResourceRef{ m_name };
        };

    protected:
        auto RegisterSound() -> bool;

    private:
        AZ::Name m_name{};
        MiniAudio::SoundDataAsset m_soundAsset{};
        bool m_registered{};
    };
} // namespace BopAudio
