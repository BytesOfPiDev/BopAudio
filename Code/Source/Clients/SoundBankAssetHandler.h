#pragma once

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"

namespace BopAudio
{
    class SoundBankAssetHandler
        : public AZ::Data::AssetHandler
        , public AZ::AssetTypeInfoBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY_MOVE(SoundBankAssetHandler);
        AZ_TYPE_INFO_WITH_NAME_DECL(SoundBankAssetHandler);

        SoundBankAssetHandler();
        ~SoundBankAssetHandler() override;

        void Register();
        void Unregister();

        auto CreateAsset(const AZ::Data::AssetId& id, const AZ::Data::AssetType& type)
            -> AZ::Data::AssetPtr override;
        auto LoadAssetData(
            AZ::Data::Asset<AZ::Data::AssetData> const& asset,
            AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
            AZ::Data::AssetFilterCB const& assetLoadFilterCB)
            -> AZ::Data::AssetHandler::LoadResult override;
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
    };
} // namespace BopAudio
