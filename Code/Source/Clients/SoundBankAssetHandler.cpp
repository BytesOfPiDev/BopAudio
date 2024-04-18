#include "SoundBankAssetHandler.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/Utils.h"

#include "Clients/SoundBankAsset.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(SoundBankAssetHandler, AZ::SystemAllocator);
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        SoundBankAssetHandler, "SoundBankAssetHandler", "{3F1E6E35-4791-4A6A-81B7-D57DAF82E761}");
    AZ_RTTI_NO_TYPE_INFO_IMPL(SoundBankAssetHandler, AZ::Data::AssetHandler);

    SoundBankAssetHandler::SoundBankAssetHandler()
    {
        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::AzTypeInfo<SoundBankAsset>::Uuid());
    }

    SoundBankAssetHandler::~SoundBankAssetHandler()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();
    }

    void SoundBankAssetHandler::Register()
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequestBus::Events::EnableCatalogForAsset,
            AZ::AzTypeInfo<SoundBankAsset>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequestBus::Events::AddExtension,
            SoundBankAsset::ProductExtension);

        AZ_Assert(AZ::Data::AssetManager::IsReady(), "AssetManager isn't ready!");
        AZ::Data::AssetManager::Instance().RegisterHandler(
            this, AZ::AzTypeInfo<SoundBankAsset>::Uuid());
    }

    void SoundBankAssetHandler::Unregister()
    {
        if (AZ::Data::AssetManager::IsReady())
        {
            AZ::Data::AssetManager::Instance().UnregisterHandler(this);
        }
    }

    auto SoundBankAssetHandler::CreateAsset(
        AZ::Data::AssetId const& /*id*/, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (type == AZ::AzTypeInfo<SoundBankAsset>::Uuid())
        {
            return aznew SoundBankAsset{};
        }

        AZ_Error("SoundBankAssetHandler", false, "The type requested is not supported.");
        return {};
    }

    auto SoundBankAssetHandler::LoadAssetData(
        AZ::Data::Asset<AZ::Data::AssetData> const& asset,
        AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
        AZ::Data::AssetFilterCB const& /*assetLoadFilterCB*/) -> AZ::Data::AssetHandler::LoadResult
    {
        bool const assetLoaded = AZ::Utils::LoadObjectFromStreamInPlace<SoundBankAsset>(
            *stream, *asset.GetAs<SoundBankAsset>());

        if (!assetLoaded)
        {
            AZ_Error("SoundBankAssetHandler", false, "Failed to load asset.");
            return AZ::Data::AssetHandler::LoadResult::Error;
        }

        return AZ::Data::AssetHandler::LoadResult::LoadComplete;
    }

    void SoundBankAssetHandler::DestroyAsset(AZ::Data::AssetPtr ptr)
    {
        delete ptr;
    }

    void SoundBankAssetHandler::GetHandledAssetTypes(AZStd::vector<AZ::Data::AssetType>& assetTypes)
    {
        assetTypes.push_back(AZ::AzTypeInfo<SoundBankAsset>::Uuid());
    }

    auto SoundBankAssetHandler::GetAssetType() const -> AZ::Data::AssetType
    {
        return AZ::AzTypeInfo<SoundBankAsset>::Uuid();
    }

    void SoundBankAssetHandler::GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions)
    {
        extensions.push_back(SoundBankAsset::ProductExtension);
        extensions.push_back(SoundBankAsset::SourceExtension);
    }

    auto SoundBankAssetHandler::GetAssetTypeDisplayName() const -> char const*
    {
        return "SoundBank Asset";
    }

    auto SoundBankAssetHandler::GetBrowserIcon() const -> char const*
    {
        return {};
    }

    auto SoundBankAssetHandler::GetGroup() const -> char const*
    {
        return SoundBankAsset::AssetGroup;
    }

    auto SoundBankAssetHandler::GetComponentTypeId() const -> AZ::Uuid
    {
        return {};
    }

    auto SoundBankAssetHandler::CanCreateComponent(AZ::Data::AssetId const& /*assetId*/) const
        -> bool
    {
        return false;
    }

} // namespace BopAudio
