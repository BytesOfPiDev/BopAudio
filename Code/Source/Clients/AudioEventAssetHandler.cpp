#include "Clients/AudioEventAssetHandler.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/Serialization/Utils.h"

#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAssetHandler, Audio::AudioImplAllocator);

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAssetHandler, "AudioEventAssetHandler", "{AD28F187-2EA4-465E-BB3E-6854696CB135}");

    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAssetHandler, AZ::Data::AssetHandler);

    AudioEventAssetHandler::AudioEventAssetHandler()
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset,
            AZ::AzTypeInfo<AudioEventAsset>::Uuid());

        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::AddExtension, AudioEventAsset::ProductExtension);

        Register();
    }

    AudioEventAssetHandler::~AudioEventAssetHandler()
    {
        Unregister();
    }

    void AudioEventAssetHandler::Register()
    {
        if (!AZ::Data::AssetManager::IsReady())
        {
            AZ_Error(
                "AudioEventAssetHandler",
                false,
                "The Asset Manager isn't ready. It is required in order to "
                "handle assets.");

            return;
        }

        AZ::Data::AssetManager::Instance().RegisterHandler(
            this, AZ::AzTypeInfo<AudioEventAsset>::Uuid());

        AZ::AssetTypeInfoBus::Handler::BusConnect(AZ::AzTypeInfo<AudioEventAsset>::Uuid());
    }

    void AudioEventAssetHandler::Unregister()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect();

        if (!AZ::Data::AssetManager::IsReady())
        {
            return;
        }

        AZ::Data::AssetManager::Instance().UnregisterHandler(this);
    }

    auto AudioEventAssetHandler::CreateAsset(
        AZ::Data::AssetId const& /*id*/, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (aztypeid_cmp(type, AZ::AzTypeInfo<AudioEventAsset>::Uuid()))
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
            AZ_Error("AudioEventAssetHandler", false, "Failed to load asset.");
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
        assetTypes.push_back(AZ::AzTypeInfo<AudioEventAsset>::Uuid());
    }

    auto AudioEventAssetHandler::GetAssetType() const -> AZ::Data::AssetType
    {
        return AZ::AzTypeInfo<AudioEventAsset>::Uuid();
    }

    void AudioEventAssetHandler::GetAssetTypeExtensions(AZStd::vector<AZStd::string>& extensions)
    {
        extensions.push_back(AudioEventAsset::ProductExtension);
        extensions.push_back(AudioEventAsset::SourceExtension);
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
