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
    AZ_RTTI_NO_TYPE_INFO_IMPL(SoundBankAsset, AZ::Data::AssetData);
    AZ_TYPE_INFO_WITH_NAME_IMPL(SoundBankAsset, "SoundBankAsset", SoundBankAssetTypeId);
    AZ_CLASS_ALLOCATOR_IMPL(SoundBankAsset, Audio::AudioImplAllocator);

    void SoundBankAsset::Reflect(AZ::ReflectContext* context)
    {
        AudioEventAsset::Reflect(context);
        AudioEventId::Reflect(context);
        BankRef::Reflect(context);
        ResourceRef::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SoundBankAsset, AZ::Data::AssetData>()
                ->Version(1)
                ->Field("Id", &SoundBankAsset::m_id)
                ->Field("Sounds", &SoundBankAsset::m_soundSources);

            serialize->RegisterGenericType<AZ::Data::Asset<SoundBankAsset>>();
            serialize->RegisterGenericType<AudioEventId>();
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

    auto SoundBankAsset::CloneEvent(AudioEventId) const -> AudioOutcome<AudioEventAsset>
    {
        return AZ::Failure("Unimplemented");
    }

    auto SoundBankAsset::CloneEvent(ResourceRef const& resourceId) const
        -> AudioOutcome<AudioEventAsset>
    {
        return CloneEvent(AudioEventId{ resourceId.GetAsPath().Filename().Native() });
    }

} // namespace BopAudio
