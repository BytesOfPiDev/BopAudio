#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetSerializer.h"
#include "Clients/AudioEventAsset.h"
#include "MiniAudio/SoundAsset.h"

#include "BopAudio/Util.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"

namespace BopAudio
{
    class SoundBankAsset : public AZ::Data::AssetData
    {
        friend class SoundBankAssetBuilderWorker;
        using SoundAssetMap = AZStd::unordered_map<ResourceRef, SoundSource>;

    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY_MOVE(SoundBankAsset);
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(SoundBankAsset);

        static constexpr auto SourceExtension = ".soundbankdata";
        static constexpr auto ProductExtension = SoundbankExtension;
        static constexpr auto AssetGroup = "Sound";
        static constexpr auto AssetSubId = 1u;

        SoundBankAsset();
        ~SoundBankAsset() override = default;

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] constexpr auto operator==(ResourceRef const& resourceId) const
        {
            return (m_id.IsValid() && m_id.IsValid()) && (m_id.GetHash() == resourceId.GetHash());
        }

        [[nodiscard]] constexpr auto operator==(BankRef const& resourceId) const
        {
            return m_id == resourceId;
        }

        [[nodiscard]] auto GetResourceId() const -> BankRef
        {
            return m_id;
        }

        [[nodiscard]] auto GetSoundSources() const -> AZStd::vector<SoundSource> const&
        {
            return m_soundSources;
        }

        [[nodiscard]] auto GetSoundAsset(ResourceRef const& soundName) const
            -> MiniAudio::SoundDataAsset;

        [[nodiscard]] auto IsEmpty() const -> bool
        {
            return m_events.empty() && m_soundSources.empty();
        };

        [[nodiscard]] auto CloneEvent(AudioEventId eventId) const -> AudioOutcome<AudioEventAsset>;
        [[nodiscard]] auto CloneEvent(ResourceRef const& resourceId) const
            -> AudioOutcome<AudioEventAsset>;

    private:
        BankRef m_id{};

        AZStd::vector<SoundSource> m_soundSources{};
        AZStd::vector<AudioEventAsset> m_events{};
        AudioEventIdContainer m_eventIds{};
    };

    using AudioAssetVector = AZStd::vector<AZ::Data::Asset<SoundBankAsset>>;
} // namespace BopAudio
