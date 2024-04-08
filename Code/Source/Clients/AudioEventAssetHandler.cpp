#include "Clients/AudioEventAssetHandler.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Serialization/Utils.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAssetHandler, Audio::AudioImplAllocator);
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAssetHandler, "AudioEventAssetHandler", "{AD28F187-2EA4-465E-BB3E-6854696CB135}");
    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAssetHandler, AZ::Data::AssetHandler);

    AudioEventAssetHandler::AudioEventAssetHandler()
    {
        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::Data::AssetType{ AudioEventAssetTypeId });
    }

    AudioEventAssetHandler::~AudioEventAssetHandler()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();
    }

    void AudioEventAssetHandler::Register()
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequestBus::Events::EnableCatalogForAsset, AudioEventAssetType);
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequestBus::Events::AddExtension,
            AudioEventAsset::ProductExtension);

        AZ_Assert(AZ::Data::AssetManager::IsReady(), "AssetManager isn't ready!");
        AZ::Data::AssetManager::Instance().RegisterHandler(this, AudioEventAssetType);
    }

    void AudioEventAssetHandler::Unregister()
    {
        if (AZ::Data::AssetManager::IsReady())
        {
            AZ::Data::AssetManager::Instance().UnregisterHandler(this);
        }
    }

    auto AudioEventAssetHandler::CreateAsset(
        AZ::Data::AssetId const& /*id*/, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (aztypeid_cmp(type, AZ::Data::AssetType{ AudioEventAssetTypeId }))
        {
            return aznew AudioEventAsset{};
        }

        AZ_Error("AudioEventAssetHandler", false, "The type requested is not supported.");

        return nullptr;
    }

    auto AudioEventAssetHandler::LoadAssetData(
        AZ::Data::Asset<AZ::Data::AssetData> const& asset,
        AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
        AZ::Data::AssetFilterCB const& /*assetLoadFilterCB*/) -> AZ::Data::AssetHandler::LoadResult
    {
        auto& audioEventAsset{ *asset.GetAs<AudioEventAsset>() };

        bool const assetLoaded =
            AZ::Utils::LoadObjectFromStreamInPlace<AudioEventAsset>(*stream, audioEventAsset);

        if (!assetLoaded)
        {
            AZ_Error("AudioEventAssetHandler", false, "Failed to load given asset.");
            return AZ::Data::AssetHandler::LoadResult::Error;
        }

        audioEventAsset.RegisterAudioEvent();

        return AZ::Data::AssetHandler::LoadResult::LoadComplete;
    }

    void AudioEventAssetHandler::DestroyAsset(AZ::Data::AssetPtr ptr)
    {
        delete ptr;
    }

    void AudioEventAssetHandler::GetHandledAssetTypes(
        AZStd::vector<AZ::Data::AssetType>& assetTypes)
    {
        assetTypes.push_back(AZ::Data::AssetType{ AudioEventAssetTypeId });
    }

    auto AudioEventAssetHandler::GetAssetType() const -> AZ::Data::AssetType
    {
        return AZ::Data::AssetType{ AudioEventAssetTypeId };
    }

    void AudioEventAssetHandler::GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions)
    {
        extensions.push_back(AudioEventAsset::ProductExtension);
    }

    auto AudioEventAssetHandler::GetAssetTypeDisplayName() const -> char const*
    {
        return "Audio Event Asset";
    }

    auto AudioEventAssetHandler::GetBrowserIcon() const -> char const*
    {
        return {};
    }

    auto AudioEventAssetHandler::GetGroup() const -> char const*
    {
        return AudioEventAsset::AssetGroup;
    }

    auto AudioEventAssetHandler::GetComponentTypeId() const -> AZ::Uuid
    {
        return {};
    }

    auto AudioEventAssetHandler::CanCreateComponent(AZ::Data::AssetId const& /*assetId*/) const
        -> bool
    {
        return false;
    }

} // namespace BopAudio
