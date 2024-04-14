#include "Clients/AudioEventAssetHandler.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Serialization/Utils.h"
#include "AzFramework/Asset/GenericAssetHandler.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAssetHandler, Audio::AudioImplAllocator);
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAssetHandler, "AudioEventAssetHandler", "{AD28F187-2EA4-465E-BB3E-6854696CB135}");
    AZ_RTTI_NO_TYPE_INFO_IMPL(
        AudioEventAssetHandler, AzFramework::GenericAssetHandlerBase, AZ::Data::AssetHandler);

    namespace Internal
    {
        auto IsProductAsset(AZ::IO::PathView assetPath) -> bool
        {
            if (assetPath.empty())
            {
                return false;
            }
            return assetPath.Match(AudioEventAsset::ProductExtensionPattern);
        }

        auto IsProductAsset(AZ::Data::AssetId assetId) -> bool
        {
            auto const assetPath{ GetAssetPath(assetId) };
            if (assetPath.empty())
            {
                return false;
            }
            return assetPath.Match(AudioEventAsset::ProductExtensionPattern);
        }

        auto IsSourceAsset(AZ::IO::PathView assetPath) -> bool
        {
            if (assetPath.empty())
            {
                return false;
            }
            return assetPath.Match(AudioEventAsset::SourceExtensionPattern);
        }

        auto IsSourceAsset(AZ::Data::AssetId assetId) -> bool
        {
            auto const assetPath{ GetAssetPath(assetId) };
            if (assetPath.empty())
            {
                return false;
            }
            return assetPath.Match(AudioEventAsset::SourceExtensionPattern);
        }

    } // namespace Internal

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
            AudioEventAsset::ProductExtensionPattern);

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

    auto AudioEventAssetHandler::CanHandleAsset(AZ::Data::AssetId const& id) const -> bool
    {
        AZ::IO::Path const assetPath = GetAssetPath(id);
        return Internal::IsProductAsset(assetPath) || Internal::IsSourceAsset(assetPath);
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
        auto* audioEventAssetData{ asset.GetAs<AudioEventAsset>() };

        AZ_Error(
            "AudioEventAssetHandler",
            audioEventAssetData != nullptr,
            "Asset is of the wrong type.");

        if (Internal::IsProductAsset(asset.GetId()))
        {
            bool const assetLoaded = AZ::Utils::LoadObjectFromStreamInPlace<AudioEventAsset>(
                *stream, *audioEventAssetData);

            if (!assetLoaded)
            {
                AZ_Error("AudioEventAssetHandler", false, "Failed to load given asset.");
                return AZ::Data::AssetHandler::LoadResult::Error;
            }
        }
        else if (Internal::IsSourceAsset(asset.GetId()))
        {
            AZ_Error(
                "AudioEventAssetHandler",
                false,
                "LoadAssetData for SourceAsset is not yet implemented.");
        }

        //        audioEventAssetData->RegisterAudioEvent();

        return AZ::Data::AssetHandler::LoadResult::LoadComplete;
    }

    auto AudioEventAssetHandler::SaveAssetData(
        const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream) -> bool
    {
        auto* audioEventAssetData = asset.GetAs<AudioEventAsset>();
        AZ_Error(
            "AudioEventAssetHandler",
            audioEventAssetData != nullptr,
            "Asset is of the wrong type.");

        AZ::SerializeContext* const serializeContext = []() -> decltype(serializeContext)
        {
            auto result{ decltype(serializeContext){} };
            AZ::ComponentApplicationBus::BroadcastResult(
                result, &AZ::ComponentApplicationBus::Events::GetSerializeContext);

            return result;
        }();

        if (audioEventAssetData && serializeContext)
        {
            return AZ::Utils::SaveObjectToStream(
                *stream, AZ::ObjectStream::ST_JSON, audioEventAssetData, serializeContext);
        }

        return false;
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
        extensions.push_back(AudioEventAsset::ProductExtensionPattern);
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
