#pragma once

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/Memory/SystemAllocator.h>

#include "AzCore/Asset/AssetCommon.h"

#include "Engine/SoundSource.h"
#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    class AudioObject;

    struct PlaySoundTask final : public IAudioTask
    {
        AZ_CLASS_ALLOCATOR(PlaySoundTask, AZ::SystemAllocator);
        AZ_DEFAULT_COPY_MOVE(PlaySoundTask);
        AZ_RTTI(PlaySoundTask, "{CF25AD9C-001B-4571-BC55-230A8A1DB6B9}", IAudioTask)

        PlaySoundTask() = default;
        ~PlaySoundTask() override = default;
        static void Reflect(AZ::ReflectContext* context);

        void operator()(AudioObject&) const override;
        void Reset() override{};

        SoundSource m_soundSource;
    };

} // namespace BopAudio
