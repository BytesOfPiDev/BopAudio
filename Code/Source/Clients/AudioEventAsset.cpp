#include "Clients/AudioEventAsset.h"

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/EditContextConstants.inl"

#include "BopAudio/Util.h"

namespace BopAudio
{
    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()->Version(5)->Attribute(
                AZ::Edit::Attributes::EnableForAssetEditor, true);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("Audio Event Asset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    AudioEventAsset::AudioEventAsset() = default;

    AudioEventAsset::~AudioEventAsset() = default;
    void StartEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StartEventData>()
                ->Version(0)
                ->Field("AudioEngineEventId", &StartEventData::m_audioEventId)
                ->Field("AudioEngineObjectId", &StartEventData::m_audioObjectId);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StartEventData>("Start AudioEvent Data", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<StartEventData>("StartAudioEventData")
                ->Attribute(
                    AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::ConstructibleFromNil, true)
                ->Attribute(AZ::Script::Attributes::EnableAsScriptEventParamType, true)
                ->Attribute(AZ::Script::Attributes::EnableAsScriptEventReturnType, true)
                ->Property("AudioEngineEventId", &StartEventData::GetAudioEventId, nullptr)
                ->Property("AudioEngineObjectId", &StartEventData::GetAudioObjectId, nullptr);
        }
    }

    void StopEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StopEventData>()->Version(0);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StopEventData>("Stop AudioEvent Data", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            auto classBuilder{ behaviorContext->Class<StopEventData>("StopAudioEventData") };

            EnableTypeForScriptEventUsage(classBuilder)
                ->Property("AudioEngineEventId", &StopEventData::GetAudioEventId, nullptr)
                ->Property("AudioEngineObjectId", &StopEventData::GetAudioObjectId, nullptr);
        }
    }
} // namespace BopAudio
