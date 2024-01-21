#include "AudioAssetHandler.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/Utils.h"

#include "BopAudio/AudioAsset.h"
#include "BopAudio/BopAudioTypeIds.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(BopAudioAssetHandler, AZ::SystemAllocator); // NOLINT

    BopAudioAssetHandler::BopAudioAssetHandler()
    {
        Register();
    }

    BopAudioAssetHandler::~BopAudioAssetHandler()
    {
        Unregister();
    }

    void BopAudioAssetHandler::Register()
    {
        if (!AZ::Data::AssetManager::IsReady())
        {
            AZ_Error("BopAudioAssetHandler", false, "The Asset Manager isn't ready. It is required in order to handle assets.");
            return;
        }

        AZ::Data::AssetManager::Instance().RegisterHandler(this, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::AzTypeInfo<AudioAsset>::Uuid());
    }

    void BopAudioAssetHandler::Unregister()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();

        if (!AZ::Data::AssetManager::IsReady())
        {
            return;
        }

        AZ::Data::AssetManager::Instance().UnregisterHandler(this);
    }

    auto BopAudioAssetHandler::CreateAsset(AZ::Data::AssetId const& id, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (aztypeid_cmp(type, AZ::AzTypeInfo<AudioAsset>::Uuid()))
        {
            return aznew AudioAsset{};
        }

        AZ_Error("AudioAssetHandler", false, "The type requested is not supported."); // NOLINT
        return {};
    }

    auto BopAudioAssetHandler::LoadAssetData(
        AZ::Data::Asset<AZ::Data::AssetData> const& asset,
        AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
        AZ::Data::AssetFilterCB const& assetLoadFilterCB) -> AZ::Data::AssetHandler::LoadResult
    {
        bool const assetLoaded = AZ::Utils::LoadObjectFromStreamInPlace<AudioAsset>(*stream, *asset.GetAs<AudioAsset>());

        if (!assetLoaded)
        {
            AZ_Error("AudioAssetHandler", false, "Failed to load asset."); // NOLINT
            return AZ::Data::AssetHandler::LoadResult::Error;
        }

        return AZ::Data::AssetHandler::LoadResult::LoadComplete;
    }

    void BopAudioAssetHandler::DestroyAsset(AZ::Data::AssetPtr ptr)
    {
        delete ptr; // NOLINT
    }

    void BopAudioAssetHandler::GetHandledAssetTypes(AZStd::vector<AZ::Data::AssetType>& assetTypes)
    {
        assetTypes.push_back(AZ::AzTypeInfo<AudioAsset>::Uuid());
    }

    auto BopAudioAssetHandler::GetAssetType() const -> AZ::Data::AssetType
    {
        return AZ::AzTypeInfo<AudioAsset>::Uuid();
    }

    void BopAudioAssetHandler::GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions)
    {
        return extensions.push_back(AudioAsset::FileExtension);
    }

    auto BopAudioAssetHandler::GetAssetTypeDisplayName() const -> const char*
    {
        return "Steam Audio Asset";
    }

    auto BopAudioAssetHandler::GetBrowserIcon() const -> char const*
    {
        return {};
    }

    auto BopAudioAssetHandler::GetGroup() const -> const char*
    {
        return AudioAsset::AssetGroup;
    }

    auto BopAudioAssetHandler::GetComponentTypeId() const -> AZ::Uuid
    {
        return AZ::Uuid(BopAudioComponentTypeId);
    }

    auto BopAudioAssetHandler::CanCreateComponent(AZ::Data::AssetId const& assetId) const -> bool
    {
        return false;
    }

} // namespace BopAudio
