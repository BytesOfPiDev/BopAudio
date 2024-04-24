#include "Clients/AudioEventAssetHandler.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Serialization/Utils.h"
#include "AzFramework/Asset/GenericAssetHandler.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Asset/AssetTypeInfoBus.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/IO/GenericStreams.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/Utils.h>

#include <AzCore/StringFunc/StringFunc.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/Asset/AssetCatalogBus.h>
#include <AzFramework/StringFunc/StringFunc.h>
#include <IAudioSystem.h>

#include "BopAudio/BopAudioTypeIds.h"
#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAssetHandler, AZ::SystemAllocator);
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
        AZ::ComponentApplicationBus::BroadcastResult(
            m_serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);

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
        return Internal::IsProductAsset(assetPath);
    }

    void AudioEventAssetHandler::InitAsset(
        AZ::Data::Asset<AZ::Data::AssetData> const& asset,
        bool /*loadStageSucceeded*/,
        bool isReload)
    {
        AZStd::string const eventIdName = [&]() -> decltype(eventIdName)
        {
            auto result{ decltype(eventIdName){} };
            AZ::StringFunc::Path::GetFileName(asset.GetHint().c_str(), result);
            AZ::StringFunc::Path::StripExtension(result);

            return result;
        }();

        auto* audioAssetData{ asset.GetAs<AudioEventAsset>() };

        audioAssetData->m_id = Audio::AudioStringToID<AudioEventId>(eventIdName.c_str());

        if (isReload)
        {
            audioAssetData->UnregisterAudioEvent();
        }

        audioAssetData->RegisterAudioEvent();

        AZ::Data::AssetManagerBus::Broadcast(
            &AZ::Data::AssetManagerBus::Events::OnAssetReady, asset);
    }

    auto AudioEventAssetHandler::CreateAsset(
        AZ::Data::AssetId const& /*id*/, AZ::Data::AssetType const& type) -> AZ::Data::AssetPtr
    {
        if (type == AZ::Data::AssetType{ AudioEventAssetTypeId })
        {
            return aznew AudioEventAsset{};
        }

        AZ_Error("AudioEventAssetHandler", false, "The type requested is not supported.");
        return nullptr;
    }

    auto AudioEventAssetHandler::LoadAssetData(
        AZ::Data::Asset<AZ::Data::AssetData> const& asset,
        AZStd::shared_ptr<AZ::Data::AssetDataStream> stream,
        AZ::Data::AssetFilterCB const& assetLoadFilterCB) -> AZ::Data::AssetHandler::LoadResult
    {
        auto* assetData = asset.GetAs<AudioEventAsset>();
        AZ_Assert(assetData, "Asset is of the wrong type.");
        AZ_Assert(m_serializeContext, "Cached SerializeContext pointer is null!") if (assetData)
        {
            return AZ::Utils::LoadObjectFromStreamInPlace<AudioEventAsset>(
                       *stream,
                       *assetData,
                       m_serializeContext,
                       AZ::ObjectStream::FilterDescriptor(assetLoadFilterCB))
                ? AZ::Data::AssetHandler::LoadResult::LoadComplete
                : AZ::Data::AssetHandler::LoadResult::Error;
        }

        return AZ::Data::AssetHandler::LoadResult::Error;
    }

    auto AudioEventAssetHandler::SaveAssetData(
        const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream) -> bool
    {
        auto* assetData = asset.GetAs<AudioEventAsset>();
        AZ_Assert(assetData, "Asset is of the wrong type.");
        AZ_Assert(
            m_serializeContext,
            "Cached SerializeContext pointer is null!") if (assetData && m_serializeContext)
        {
            return AZ::Utils::SaveObjectToStream<AudioEventAsset>(
                *stream, AZ::ObjectStream::ST_JSON, assetData, m_serializeContext);
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
