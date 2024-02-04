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

        SATLAudioObjectData_BopAudio(Audio::TAudioObjectID id)
            : m_audioObjectId(id)
            , m_objectName{}
        {
        }

        ~SATLAudioObjectData_BopAudio() override = default;

        [[nodiscard]] constexpr auto GetAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_audioObjectId;
        }

        constexpr void SetAtlAudioObjectId(Audio::TAudioObjectID) = delete;

        [[nodiscard]] constexpr auto GetInstanceId() const -> UniqueId
        {
            return m_instanceId;
        }

        constexpr void ChangeName(AZStd::string_view objectName)
        {
            m_objectName = objectName;
        }

    private:
        Audio::TAudioObjectID m_audioObjectId;
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

        [[nodiscard]] constexpr auto GetTriggerId() const -> ResourceId
        {
            return m_triggerId;
        }

        constexpr void SetTriggerId(ResourceId resourceId)
        {
            m_triggerId = resourceId;
        }

    private:
        ResourceId m_triggerId{};
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

    struct SATLEventData_BopAudio : public Audio::IATLEventData
    {
        explicit SATLEventData_BopAudio(Audio::TAudioEventID const passedId)
            : m_baId{}
            , m_atlId(passedId)
            , m_sourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        ~SATLEventData_BopAudio() override = default;

        Audio::EAudioEventState m_audioEventState{};
        UniqueId m_baId{};
        Audio::TAudioEventID m_atlId{};
        Audio::TAudioSourceId m_sourceId{};
        SoundPtr m_soundInstance{};
    };

    struct SATLAudioFileEntryData_BopAudio : public Audio::IATLAudioFileEntryData
    {
        SATLAudioFileEntryData_BopAudio() = default;

        ~SATLAudioFileEntryData_BopAudio() override
        {
            m_sounds.clear();
            m_soundAssets.clear();
            m_soundNames.clear();
        };

        ResourceId m_bankId{};
        AZStd::unordered_set<ResourceId> m_soundNames{};
        AZStd::unordered_map<ResourceId, MiniAudio::SoundDataAsset> m_soundAssets{};
        AZStd::unordered_map<ResourceId, SoundPtr> m_sounds{};
    };

} // namespace BopAudio
