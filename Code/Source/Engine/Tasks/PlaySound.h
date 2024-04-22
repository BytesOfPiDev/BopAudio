#pragma once

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <MiniAudio/SoundAsset.h>

#include "AzCore/Asset/AssetCommon.h"

#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    class AudioObject;

    struct PlaySoundTask final : public IAudioTask
    {
        AZ_DEFAULT_COPY_MOVE(PlaySoundTask);
        AZ_RTTI(PlaySoundTask, "{CF25AD9C-001B-4571-BC55-230A8A1DB6B9}", IAudioTask)

        PlaySoundTask() = default;
        ~PlaySoundTask() override = default;
        static void Reflect(AZ::ReflectContext* context);

        void operator()(AudioObject&) const override;
        void Reset() override{};

        MiniAudio::SoundDataAsset m_soundSource;
    };

} // namespace BopAudio
