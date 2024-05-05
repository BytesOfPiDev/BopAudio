#include "Clients/ParameterTypes.h"

#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"

#include "BopAudio/Util.h"

namespace BopAudio
{
    void StartEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StartEventData>()
                ->Version(0)
                ->Field("Eventid", &StartEventData::m_audioEventId)
                ->Field("ObjectId", &StartEventData::m_audioEventId);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StartEventData>("StartEventData", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            auto classBuilder{ behaviorContext->Class<StartEventData>("StartAudioEventData") };

            EnableTypeForScriptEventUsage(classBuilder)
                ->Property("EventId", &StartEventData::GetAudioEventId, nullptr)
                ->Property("ObjectId", &StartEventData::GetAudioObjectId, nullptr);
        }
    }

    void StopEventData::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StopEventData>()
                ->Version(0)
                ->Field("EventId", &StopEventData::m_audioEventId)
                ->Field("ObjectId", &StopEventData::m_audioObjectId);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<StopEventData>("StopAudioEventData", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            auto classBuilder{ behaviorContext->Class<StopEventData>("StopAudioEventData") };

            EnableTypeForScriptEventUsage(classBuilder)
                ->Property("EventId", &StopEventData::GetAudioEventId, nullptr)
                ->Property("ObjectId", &StopEventData::GetAudioObjectId, nullptr);
        }
    }
} // namespace BopAudio
