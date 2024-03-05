#pragma once

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"

#include "Engine/Id.h"
#include "Engine/Sound.h"
#include "Engine/Tasks/AudioTaskBase.h"
#include "Engine/Tasks/TaskBus.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    class AudioObject;

    struct PlaySoundTask : public AudioTaskBase<PlaySoundTask>
    {
        AZ_TYPE_INFO_WITH_NAME_DECL(PlaySoundTask);
        AZ_CLASS_ALLOCATOR_DECL;

        static void Reflect(AZ::ReflectContext* context);

        static auto CreateFactory() -> AZStd::unique_ptr<TaskFactoryRequests>;

        void operator()(AudioObject&) const;

        ResourceRef m_resourceToPlay{};
    };

} // namespace BopAudio
