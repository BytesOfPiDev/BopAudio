#pragma once

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    class AudioEventAssetHandler
        : public AZ::Data::AssetHandler
        , public AZ::AssetTypeInfoBus::Handler
    {
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

        [[nodiscard]] auto CanHandleAsset(AZ::Data::AssetId const& id) const -> bool override
        {
            AZStd::string assetPath;
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                assetPath, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetPathById, id);
            if (!assetPath.empty())
            {
                AZStd::string assetExtension;
                if (AZ::StringFunc::Path::GetExtension(assetPath.c_str(), assetExtension, false))
                {
                    return assetExtension == AudioEventAsset::ProductExtension;
                }
            }

            return false;
        }
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
