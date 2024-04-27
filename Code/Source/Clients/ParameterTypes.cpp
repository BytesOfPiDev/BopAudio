#include "Clients/ParameterTypes.h"

#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"

#include "BopAudio/Util.h"

namespace BopAudio
{
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