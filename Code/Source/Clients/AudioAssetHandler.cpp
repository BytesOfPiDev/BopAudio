#include "AudioAssetHandler.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/Utils.h"

#include "SteamAudio/SoundAsset.h"
#include "SteamAudio/SteamAudioTypeIds.h"

namespace SteamAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(SteamAudioAssetHandler, AZ::SystemAllocator); // NOLINT

    SteamAudioAssetHandler::SteamAudioAssetHandler()
    {
        Register();
    }

    SteamAudioAssetHandler::~SteamAudioAssetHandler()
    {
        Unregister();
    }

    void SteamAudioAssetHandler::Register()
    {
        if (!AZ::Data::AssetManager::IsReady())
        {
            AZ_Error("SteamAudioAssetHandler", false, "The Asset Manager isn't ready. It is required in order to handle assets.");
            return;
        }

        AZ::Data::AssetManager::Instance().RegisterHandler(this, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::AzTypeInfo<AudioAsset>::Uuid());
    }

    void SteamAudioAssetHandler::Unregister()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();

        if (!AZ::Data::AssetManager::IsReady())
        {
            return;
        }

        AZ::Data::AssetManager::Instance().UnregisterHandler(this);
    }

    auto SteamAudioAssetHandler::CreateAsset(AZ::Data::AssetId const& id, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (aztypeid_cmp(type, AZ::AzTypeInfo<AudioAsset>::Uuid()))
        {
            return aznew AudioAsset{};
        }

        AZ_Error("AudioAssetHandler", false, "The type requested is not supported."); // NOLINT
        return {};
    }

    auto SteamAudioAssetHandler::LoadAssetData(
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

    void SteamAudioAssetHandler::DestroyAsset(AZ::Data::AssetPtr ptr)
    {
        delete ptr; // NOLINT
    }

    void SteamAudioAssetHandler::GetHandledAssetTypes(AZStd::vector<AZ::Data::AssetType>& assetTypes)
    {
        assetTypes.push_back(AZ::AzTypeInfo<AudioAsset>::Uuid());
    }

    auto SteamAudioAssetHandler::GetAssetType() const -> AZ::Data::AssetType
    {
        return AZ::AzTypeInfo<AudioAsset>::Uuid();
    }

    void SteamAudioAssetHandler::GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions)
    {
        return extensions.push_back(AudioAsset::FileExtension);
    }

    auto SteamAudioAssetHandler::GetAssetTypeDisplayName() const -> const char*
    {
        return "Steam Audio Asset";
    }

    auto SteamAudioAssetHandler::GetBrowserIcon() const -> char const*
    {
        return {};
    }

    auto SteamAudioAssetHandler::GetGroup() const -> const char*
    {
        return AudioAsset::AssetGroup;
    }

    auto SteamAudioAssetHandler::GetComponentTypeId() const -> AZ::Uuid
    {
        return AZ::Uuid(SteamAudioComponentTypeId);
    }

    auto SteamAudioAssetHandler::CanCreateComponent(AZ::Data::AssetId const& assetId) const -> bool
    {
        return false;
    }

} // namespace SteamAudio
