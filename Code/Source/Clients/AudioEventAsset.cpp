#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>

#include "AudioAllocators.h"

#include "Engine/AudioEvent.h"
#include "Engine/AudioObject.h"
#include "Engine/Tasks/PlaySound.h"
#include "Engine/Tasks/StopSound.h"

namespace BopAudio
{
    namespace Internal
    {
        using Task = AZStd::variant<PlaySoundTask, StopSoundTask>;
        using TaskContainer = AZStd::fixed_vector<Task, MaxTasks>;
    } // namespace Internal

    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAsset, Audio::AudioImplAllocator);

    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAsset, AZ::Data::AssetData);

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAsset, "AudioEventAsset", "{F26CEC71-D882-4367-BCBF-B9B041E1C708}");

    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        Internal::TaskContainer,
        "{961FB915-9667-47CD-BA40-7C360935B7B6}",
        "AudioEventInternalTaskContainer");

    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()->Version(0)->Field(
                "Id", &AudioEventAsset::m_id);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("AudioEventAsset", "");
            }
        }
    }

    auto AudioEventAsset::Execute(AudioObject&) -> AZ::Outcome<void, char const*>
    {
        return AZ::Failure("Unimplemented");
    }

    void AudioEventAsset::operator()(AudioObject& audioObject)
    {
        Execute(audioObject);
    }

} // namespace BopAudio
