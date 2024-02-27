#include "Clients/AudioEventAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/JSON/pointer.h>

#include "AudioAllocators.h"

#include "Engine/AudioObject.h"
#include "Engine/Tasks/Common.h"

namespace BopAudio
{

    AZ_CLASS_ALLOCATOR_IMPL(AudioEventAsset, Audio::AudioImplAllocator);

    AZ_RTTI_NO_TYPE_INFO_IMPL(AudioEventAsset, AZ::Data::AssetData);

    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioEventAsset, "AudioEventAsset", "{F26CEC71-D882-4367-BCBF-B9B041E1C708}");

    void AudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<AudioEventAsset, AZ::Data::AssetData>()
                ->Version(0)
                ->Field("Id", &AudioEventAsset::m_id)
                ->Field("Tasks", &AudioEventAsset::m_tasks);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<AudioEventAsset>("AudioEventAsset", "");
            }
        }
    }

    auto AudioEventAsset::Execute(AudioObject& audioObject) const -> AZ::Outcome<void, char const*>
    {
        AZ_Error("AudioEventAsset", false, "Unimplemented");

        AZStd::ranges::for_each(
            m_tasks,
            [&audioObject](Task const& taskVariant)
            {
                AZStd::visit(
                    [&audioObject](auto const& task)
                    {
                        task(audioObject);
                    },
                    taskVariant);
            });

        return AZ::Failure("Unimplemented");
    }

    void AudioEventAsset::operator()(AudioObject& audioObject) const
    {
        Execute(audioObject);
    }

} // namespace BopAudio
