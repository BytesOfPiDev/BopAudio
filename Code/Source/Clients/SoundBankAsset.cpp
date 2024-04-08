#include "Clients/SoundBankAsset.h"

#include "AudioAllocators.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "BopAudio/BopAudioTypeIds.h"
#include "Clients/AudioEventAsset.h"
#include "Engine/Id.h"

namespace BopAudio
{
    AZ_CLASS_ALLOCATOR_IMPL(SoundBankAsset, Audio::AudioImplAllocator);
    AZ_RTTI_NO_TYPE_INFO_IMPL(SoundBankAsset, AZ::Data::AssetData);
    AZ_TYPE_INFO_WITH_NAME_IMPL(SoundBankAsset, "SoundBankAsset", SoundBankAssetTypeId);

    void SoundBankAsset::Reflect(AZ::ReflectContext* context)
    {
        ResourceRefBase::Reflect(context);
        AudioEventAsset::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<ResourceRef, ResourceRefBase>()->Version(1);
            serialize->Class<BankRef, ResourceRefBase>()->Version(1);
            serialize->Class<SoundRef, ResourceRefBase>()->Version(1);

            serialize->Class<SoundBankAsset, AZ::Data::AssetData>()
                ->Version(2)
                ->Field("Id", &SoundBankAsset::m_id)
                ->Field("Sounds", &SoundBankAsset::m_soundSources)
                ->Field("SoundData", &SoundBankAsset::m_soundData);

            serialize->RegisterGenericType<AZ::Data::Asset<SoundBankAsset>>();
            serialize->RegisterGenericType<AZStd::vector<BankRef>>();

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<SoundBankAsset>("SoundBankAsset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    SoundBankAsset::SoundBankAsset() = default;

} // namespace BopAudio
