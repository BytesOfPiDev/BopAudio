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
        SoundSource(SoundRef soundResourceRef);
        SoundSource(AZ::IO::Path localPath);

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] explicit operator size_t() const
        {
            return m_soundRef.GetHash();
        }

        [[nodiscard]] auto operator==(SoundSource const& other) const
        {
            return m_soundRef == other.m_soundRef;
        }

        [[nodiscard]] auto operator!=(SoundSource const& other) const
        {
            return !((*this) == other);
        }

        [[nodiscard]] auto GetAsset() const -> MiniAudio::SoundDataAsset
        {
            return m_soundAsset;
        }

        auto Load() -> AZ::Outcome<void, char const*>;

        [[nodiscard]] auto IsReady() const -> bool
        {
            return m_soundAsset.IsReady();
        }

        [[nodiscard]] auto GetResourceId() const -> SoundRef
        {
            return m_soundRef;
        };

    protected:
        auto RegisterSound() -> bool;
        void OnSoundAssetChanged();

    private:
        SoundRef m_soundRef{};
        MiniAudio::SoundDataAsset m_soundAsset{};
        bool m_registered{};
    };
} // namespace BopAudio
