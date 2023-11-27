#include "SteamAudio/AudioAsset.h"

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "SteamAudio/SteamAudioTypeIds.h"

namespace SteamAudio
{
    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioAsset);
    AZ_TYPE_INFO_WITH_NAME_IMPL(AudioAsset, "AudioAsset", AudioAssetTypeId);
    AZ_CLASS_ALLOCATOR_IMPL(AudioAsset, AZ::SystemAllocator);

    void AudioAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioAsset, AZ::Data::AssetData>()->Version(1)->Field("Data", &AudioAsset::m_data);

            serialize->RegisterGenericType<AZ::Data::Asset<AudioAsset>>();

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioAsset>("Audio Asset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "SteamAudio")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }
} // namespace SteamAudio
