#pragma once

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzFramework/Asset/GenericAssetHandler.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    class AudioEventAssetHandler
        : public AzFramework::GenericAssetHandlerBase
        , public AZ::AssetTypeInfoBus::Handler
    {
        using Base = AzFramework::GenericAssetHandlerBase;
        static constexpr auto AudioEventAssetType = AZ::Data::AssetType{ AudioEventAssetTypeId };

    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY_MOVE(AudioEventAssetHandler);
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEventAssetHandler);

        AudioEventAssetHandler();
        ~AudioEventAssetHandler() override;

        void Register();
        void Unregister();

        [[nodiscard]] auto CanHandleAsset(AZ::Data::AssetId const& id) const -> bool override;

        // Called after the data loading stage and after all dependencies have been fulfilled.
        // Override this if the asset needs post-load init.
        // WARN: If overriden, the handler is responsible
        // for notifying the asset manager when the asset is ready via
        // AssetDatabaseBus::OnAssetReady.
        void InitAsset(
            AZ::Data::Asset<AZ::Data::AssetData> const& asset,
            bool loadStageSucceeded,
            bool isReload) override;

        auto CreateAsset(const AZ::Data::AssetId& id, const AZ::Data::AssetType& type)
            -> AZ::Data::AssetPtr override;

        auto LoadAssetData(
            AZ::Data::Asset<AZ::Data::AssetData> const& asset,
            AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
            AZ::Data::AssetFilterCB const& assetLoadFilterCB)
            -> AZ::Data::AssetHandler::LoadResult override;

        auto SaveAssetData(
            const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream)
            -> bool override;

        void DestroyAsset(AZ::Data::AssetPtr ptr) override;

        void GetHandledAssetTypes(AZStd::vector<AZ::Data::AssetType>& assetTypes) override;

        [[nodiscard]] auto GetAssetType() const -> AZ::Data::AssetType override;
        void GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions) override;
        [[nodiscard]] auto GetAssetTypeDisplayName() const -> char const* override;
        [[nodiscard]] auto GetBrowserIcon() const -> char const* override;
        [[nodiscard]] auto GetGroup() const -> char const* override;
        [[nodiscard]] auto GetComponentTypeId() const -> AZ::Uuid override;
        [[nodiscard]] auto CanCreateComponent(AZ::Data::AssetId const& assetId) const
            -> bool override;

    private:
        AZ::SerializeContext* m_serializeContext;
    };
} // namespace BopAudio
