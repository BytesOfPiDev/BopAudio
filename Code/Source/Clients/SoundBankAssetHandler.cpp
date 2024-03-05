#include "SoundBankAssetHandler.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Serialization/Utils.h"

#include "Clients/SoundBankAsset.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        SoundBankAssetHandler, "SoundBankAssetHandler", "{3F1E6E35-4791-4A6A-81B7-D57DAF82E761}");

    AZ_CLASS_ALLOCATOR_IMPL(SoundBankAssetHandler, Audio::AudioImplAllocator, AZ::AssetTypeInfo);

    SoundBankAssetHandler::SoundBankAssetHandler()
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset,
            AZ::AzTypeInfo<SoundBankAsset>::Uuid());

        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::AddExtension, SoundBankAsset::ProductExtension);

        Register();
    }

    SoundBankAssetHandler::~SoundBankAssetHandler()
    {
        Unregister();
    }

    void SoundBankAssetHandler::Register()
    {
        if (!AZ::Data::AssetManager::IsReady())
        {
            AZ_Error(
                "SoundBankAssetHandler",
                false,
                "The Asset Manager isn't ready. It is required in order to "
                "handle assets.");
            return;
        }

        AZ::Data::AssetManager::Instance().RegisterHandler(
            this, AZ::AzTypeInfo<SoundBankAsset>::Uuid());

        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::AzTypeInfo<SoundBankAsset>::Uuid());
    }

    void SoundBankAssetHandler::Unregister()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();

        if (!AZ::Data::AssetManager::IsReady())
        {
            return;
        }

        AZ::Data::AssetManager::Instance().UnregisterHandler(this);
    }

    auto SoundBankAssetHandler::CreateAsset(
        AZ::Data::AssetId const& /*id*/, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (aztypeid_cmp(type, AZ::AzTypeInfo<SoundBankAsset>::Uuid()))
        {
            return aznew SoundBankAsset{};
        }

        AZ_Error("SoundBankAssetHandler", false,
                 "The type requested is not supported."); // NOLINT
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
