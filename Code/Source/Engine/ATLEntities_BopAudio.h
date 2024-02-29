#pragma once

#include "ATLEntityData.h"
#include "AzCore/Math/Crc.h"
#include "Engine/Id.h"
#include "Engine/Sound.h"

#include "IAudioInterfacesCommonData.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    template<typename T>
    static auto NumericIdToName(T id) -> AZ::Name
    {
        static_assert(AZStd::is_convertible_v<T, AZ::Crc32>, "Id must be convertible to AZ::u32!");

        constexpr auto IdStringFormat{ "id_%u" };
        return AZ::Name{ AZStd::string::format(IdStringFormat, static_cast<AZ::u32>(id)) };
    }

    struct SATLAudioObjectData_BopAudio : public Audio::IATLAudioObjectData
    {
        AZ_DISABLE_COPY_MOVE(SATLAudioObjectData_BopAudio);

        SATLAudioObjectData_BopAudio(Audio::TAudioObjectID atlAudioObjectId)
            : m_atlAudioObjectId(atlAudioObjectId)
            , m_objectName{}
        {
        }

        ~SATLAudioObjectData_BopAudio() override = default;

        [[nodiscard]] constexpr auto GetAtlAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_atlAudioObjectId;
        }

        constexpr void SetAtlAudioObjectId(Audio::TAudioObjectID) = delete;

        [[nodiscard]] constexpr auto GetImplAudioObjectId() const -> AudioObjectId
        {
            return m_implAudioObjectId;
        }

        [[nodiscard]] constexpr auto GetInstanceId() const -> UniqueId
        {
            return m_instanceId;
        }

        constexpr void ChangeName(AZStd::string_view objectName)
        {
            m_objectName = objectName;
        }

    private:
        Audio::TAudioObjectID m_atlAudioObjectId;
        AudioObjectId m_implAudioObjectId{};
        UniqueId m_instanceId{};
        AZ::Name m_objectName;
    };

    struct SATLTriggerImplData_BopAudio : public Audio::IATLTriggerImplData
    {
        SATLTriggerImplData_BopAudio() = default;

        [[nodiscard]] constexpr auto GetAtlAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_atlAudioObjectId;
        }

        constexpr void SetAtlAudioObjectId(Audio::TAudioObjectID atlAudioObjectId)
        {
            m_atlAudioObjectId = atlAudioObjectId;
        }

        [[nodiscard]] constexpr auto GetImplAudioObjectId() const -> AudioObjectId
        {
            return m_implAudioObjectId;
        }

        constexpr void SetImplAudioObjectId(AudioObjectId implAudioObjectIde)
        {
            m_implAudioObjectId = implAudioObjectIde;
        }

        [[nodiscard]] auto GeImplTriggerId() const -> AudioEventId
        {
            return m_audioEventResourceId;
        }

        void SetImplTriggerId(AudioEventId resourceId)
        {
            m_audioEventResourceId = resourceId;
        }

    private:
        AudioEventId m_audioEventResourceId{};
        AudioObjectId m_implAudioObjectId{};
        Audio::TAudioObjectID m_atlAudioObjectId{};
    };

    struct SATLListenerData_BopAudio : public Audio::IATLListenerData
    {
        explicit SATLListenerData_BopAudio(Audio::TAudioObjectID id)
            : m_listenerObjectId(id)
        {
        }

        Audio::TAudioObjectID m_listenerObjectId{};
    };

    class SATLEventData_BopAudio : public Audio::IATLEventData
    {
    public:
        AZ_DISABLE_COPY(SATLEventData_BopAudio);

        explicit SATLEventData_BopAudio(Audio::TAudioEventID const atlEventId)
            : m_atlEventId(atlEventId)
            , m_sourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        ~SATLEventData_BopAudio() override = default;

        /*
              [[nodiscard]] constexpr auto GetImplEventId() const -> AudioEventId
              {
                  return m_implEventId;
              }

        void SetImplEventId(AudioEventId eventId)
        {
            m_implEventId = eventId;
        }
        */

        [[nodiscard]] constexpr auto GetAtlEventId() const -> Audio::TAudioEventID
        {
            return m_atlEventId;
        }

        constexpr void SetAtlEventId(Audio::TAudioEventID atlEventId)
        {
            m_atlEventId = atlEventId;
        }

        [[nodiscard]] constexpr auto GetEventState() const -> Audio::EAudioEventState
        {
            return m_eventState;
        }

        constexpr void SetEventState(Audio::EAudioEventState atlEventState)
        {
            m_eventState = atlEventState;
        }

        [[nodiscard]] constexpr auto GetSourceId() const -> Audio::TAudioSourceId
        {
            return m_sourceId;
        }

        constexpr void SetSourceId(Audio::TAudioSourceId atlSourceId)
        {
            m_sourceId = atlSourceId;
        }

    private:
        Audio::EAudioEventState m_eventState{};
        Audio::TAudioEventID m_atlEventId{};
        Audio::TAudioSourceId m_sourceId{};
        // AudioEventId m_implEventId{};
    };

    struct SATLAudioFileEntryData_BopAudio : public Audio::IATLAudioFileEntryData
    {
        AZ_DEFAULT_COPY_MOVE(SATLAudioFileEntryData_BopAudio);

        SATLAudioFileEntryData_BopAudio() = default;

        ~SATLAudioFileEntryData_BopAudio() override
        {
            m_sounds.clear();
            m_soundAssets.clear();
            m_soundNames.clear();
        };

        ResourceRef m_bankId{};
        AZStd::unordered_set<ResourceRef> m_soundNames{};
        AZStd::unordered_map<ResourceRef, MiniAudio::SoundDataAsset> m_soundAssets{};
        AZStd::unordered_map<ResourceRef, SoundInstance> m_sounds{};
    };

    struct EventStartedData
    {
    };

    struct EventFinishedData
    {
    };

} // namespace BopAudio
