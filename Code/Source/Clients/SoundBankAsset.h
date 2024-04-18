#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetSerializer.h"
#include "Clients/AudioEventAsset.h"
#include "IAudioInterfacesCommonData.h"
#include "MiniAudio/SoundAsset.h"

#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"

namespace BopAudio
{
    class SoundBankAsset : public AZ::Data::AssetData
    {
        friend class SoundBankAssetBuilderWorker;
        friend class SoundBankAssetHandler;
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

        struct Section
        {
            AZ::u32 m_beginFrame;
            AZ::u32 m_endFrame;
        };

        SoundBankAsset();
        ~SoundBankAsset() override = default;

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto operator==(ResourceRef const& resourceId) const
        {
            return (m_id.IsValid() && m_id.IsValid()) && (m_id.GetHash() == resourceId.GetHash());
        }

        [[nodiscard]] auto operator==(BankRef const& resourceId) const
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

        [[nodiscard]] auto GetEventAssets() const
            -> AZStd::span<AZ::Data::Asset<AudioEventAsset> const>
        {
            return m_events;
        }

        [[nodiscard]] auto IsEmpty() const -> bool
        {
            return m_events.empty() && m_soundSources.empty();
        };

    protected:
        void MapControlToEvent(Audio::TAudioControlID controlId, AudioEventId eventId)
        {
            m_controlToEventMap[controlId] = AZStd::move(eventId);
        }

    private:
        BankRef m_id{};
        AZStd::vector<AZStd::byte> m_soundData{};

        AZStd::vector<SoundSource> m_soundSources{};
        AZStd::vector<AZ::Data::Asset<AudioEventAsset>> m_events{};
        AZStd::unordered_map<Audio::TAudioControlID, AudioEventId> m_controlToEventMap{};
    };

    using AudioAssetVector = AZStd::vector<AZ::Data::Asset<SoundBankAsset>>;
} // namespace BopAudio
