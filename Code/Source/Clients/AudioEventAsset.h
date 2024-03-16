#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/RTTI/TypeInfoSimple.h"

#include "Engine/AudioEventBus.h"
#include "Engine/Id.h"
#include "Engine/Tasks/Common.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    class AudioObject;

    class AudioEventAsset
        : public AZ::Data::AssetData
        , public MiniAudioEventRequestBus::Handler
    {
        friend class AudioEventAssetBuilderWorker;
        friend class AudioEventAssetHandler;
        friend class MiniAudioEngine;

    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY_MOVE(AudioEventAsset);
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEventAsset);

        static constexpr auto SourceExtension{ ".audioeventsource" };
        static constexpr auto ProductExtension{ ".audioevent" };
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

        [[nodiscard]] auto GetEventId() const -> AudioEventId
        {
            return m_id;
        }

        void operator()(AudioObject& audioObject) const;

        [[nodiscard]] auto GetTasks() const -> TaskContainer const&
        {
            return m_tasks;
        }

    protected:
        [[nodiscard]] auto GetEventState() const -> AudioEventState override
        {
            return m_eventState;
        }

        [[nodiscard]] auto TryStartEvent(AudioObject& obj) -> bool override;
        [[nodiscard]] auto TryStopEvent(AudioObject& obj) -> bool override;

        void RegisterAudioEvent();

        void UnregisterAudioEvent();

    private:
        AudioEventId m_id{};
        AZ::Name m_name{};
        TaskContainer m_tasks{};
        MiniAudio::SoundDataAssetVector m_dependentSounds{};
        AudioEventState m_eventState{};
    };

    using AudioEventAssetDataPtr = AZ::Data::Asset<AudioEventAsset>;
    using AudioEventAssets = AZStd::vector<AudioEventAssetDataPtr>;
} // namespace BopAudio
