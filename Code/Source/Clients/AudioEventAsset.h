#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Preprocessor/Enum.h>

#include "AzCore/Asset/AssetCommon.h"

#include "BopAudio/BopAudioTypeIds.h"
#include "Engine/AudioEventBus.h"
#include "Engine/Id.h"
#include "Engine/Tasks/AudioTaskBase.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    class AudioObject;

    AZ_ENUM_CLASS(AudioEventTaskType, None, Play, Stop);

    class AudioEventAsset
        : public AZ::Data::AssetData
        , public MiniAudioEventRequestBus::Handler
    {
        friend class AudioEventAssetBuilderWorker;
        friend class AudioEventAssetHandler;
        friend class MiniAudioEngine;

    public:
        AZ_CLASS_ALLOCATOR(AudioEventAsset, AZ::SystemAllocator);
        AZ_DISABLE_COPY_MOVE(AudioEventAsset);
        AZ_RTTI(AudioEventAsset, AudioEventAssetTypeId, AZ::Data::AssetData);

        static constexpr auto SourceExtension{ "audioeventsource" };
        static constexpr auto ProductExtension{ "audioevent" };
        static constexpr auto SourceExtensionPattern{ ".audioeventsource" };
        static constexpr auto ProductExtensionPattern{ ".audioevent" };
        static constexpr auto AssetGroup = "Sound";
        static constexpr auto AssetSubId = 1u;

        /*
         * Default constructor.
         *
         * TODO: Make protected after figuring out how to serialize classes with protected
         * constructor.
         */
        AudioEventAsset();
        ~AudioEventAsset() override;

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetEventName() const -> AZ::Name
        {
            return m_name;
        }

        [[nodiscard]] constexpr auto GetEventId() const -> AudioEventId
        {
            return m_id;
        }

        void operator()(AudioObject& audioObject) const;

    protected:
        [[nodiscard]] auto GetEventState() const -> AudioEventState override
        {
            return m_eventState;
        }

        [[nodiscard]] auto TryStartEvent(AudioObject& obj) -> bool override;
        [[nodiscard]] auto TryStopEvent(AudioObject& obj) -> bool override;

        void RegisterAudioEvent();

        void UnregisterAudioEvent();

        void OnTaskSelectionChanged();
        void Cleanup();

        [[nodiscard]] auto GetTaskNames() const -> AZStd::vector<AZStd::string> const;

    private:
        AudioEventId m_id{};
        AZ::Name m_name{};
        AZStd::vector<IAudioTask*> m_eventTasks{};
        MiniAudio::SoundDataAssetVector m_dependentSounds{};
        AudioEventState m_eventState{};
    };

    using AudioEventAssetDataPtr = AZ::Data::Asset<AudioEventAsset>;
    using AudioEventAssets = AZStd::vector<AudioEventAssetDataPtr>;
} // namespace BopAudio
