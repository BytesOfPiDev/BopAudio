#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Script/ScriptContextAttributes.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Serialization/EditContextConstants.inl"
#include "BopAudio/Util.h"
#include "Engine/AudioEngineEventBus.h"
#include "IAudioInterfacesCommonData.h"

#include "Engine/AudioObject.h"

namespace BopAudio
{
    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()
                ->Version(4)
                ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("Name", &AudioEventAsset::m_name);

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

    AudioEventAsset::~AudioEventAsset()
    {
        UnregisterAudioEvent();
    }

    void AudioEventAsset::operator()(AudioObject& /*audioObject*/) const
    {
        if (m_eventState != Audio::EAudioEventState::eAES_NONE)
        {
            return;
        }
    }

    auto AudioEventAsset::TryStartEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != Audio::EAudioEventState::eAES_NONE)
        {
            return false;
        }

        m_eventState = Audio::EAudioEventState::eAES_NONE;
        (*this)(obj);

        return true;
    }

    auto AudioEventAsset::TryStopEvent(AudioObject& obj) -> bool
    {
        if (m_eventState != Audio::EAudioEventState::eAES_PLAYING)
        {
            return false;
        }

        m_eventState = Audio::EAudioEventState::eAES_UNLOADING;
        (*this)(obj);
        m_eventState = Audio::EAudioEventState::eAES_NONE;

        return true;
    }

    void AudioEventAsset::RegisterAudioEvent()
    {
        if (MiniAudioEventRequestBus::Handler::BusIsConnected())
        {
            AZ_Error(
                "AudioEventAsset",
                false,
                "Unable to register audio event. Handler already exists [Id: %s | AssetId: %s].\n",
                m_name.GetCStr(),
                GetId().ToFixedString().c_str());

            return;
        }

        if (m_id == InvalidAudioEventId)
        {
            AZ_Error(
                "AudioEventAsset", false, "Unable to register audio event. The id is invalid.\n");

            return;
        }

        MiniAudioEventRequestBus::Handler::BusConnect(m_id);
    }

    void AudioEventAsset::UnregisterAudioEvent()
    {
        MiniAudioEventRequestBus::Handler::BusDisconnect();
    };

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
