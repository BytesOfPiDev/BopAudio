#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "IAudioInterfacesCommonData.h"

#include "BopAudio/BopAudioTypeIds.h"

namespace BopAudio
{
    class AudioObject;

    class AudioEventAsset : public AZ::Data::AssetData
    {
        friend class AudioEventAssetBuilderWorker;
        friend class AudioEventAssetHandler;

        using AudioEventName = AZStd::string;

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

        void operator()(AudioObject& audioObject) const;

        void RegisterAudioEvent();
        void UnregisterAudioEvent();
    };

    using AudioEventAssetDataPtr = AZ::Data::Asset<AudioEventAsset>;
    using AudioEventAssets = AZStd::vector<AudioEventAssetDataPtr>;

    struct StartEventData
    {
        AZ_TYPE_INFO(StartEventData, "{25492250-BAB6-4D5E-85F0-34B64F1F06DF}");

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetAudioEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

        [[nodiscard]] auto GetAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_audioObjectId;
        }

        void* m_owner;
        Audio::TAudioEventID m_audioEventId;
        Audio::TAudioObjectID m_audioObjectId;
    };

    struct StopEventData
    {
        AZ_TYPE_INFO(StopEventData, "{9E2A41C2-E2DD-498B-9D46-857204B20DC0}");

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetAudioEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

        [[nodiscard]] auto GetAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_audioObjectId;
        }

        void* m_owner;
        Audio::TAudioEventID m_audioEventId;
        Audio::TAudioObjectID m_audioObjectId;
    };

    static_assert(AZStd::is_pod_v<StartEventData>);
    static_assert(AZStd::is_pod_v<StopEventData>);

} // namespace BopAudio
