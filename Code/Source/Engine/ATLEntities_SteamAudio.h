#pragma once

#include <ATLEntityData.h>
#include <AudioAllocators.h>
#include <AzCore/Math/Crc.h>
#include <IAudioInterfacesCommonData.h>

namespace SteamAudio
{

    // using TAKUniqueIDVector = AZStd::vector<AkUniqueID, Audio::AudioImplStdAllocator>;

    using SA_GameObjectId = AZ::Crc32;
    using SA_RtpcId = AZ::Crc32;
    using SA_UniqueId = AZ::Crc32;
    using SA_LibraryId = AZ::Crc32;

    struct SATLAudioObjectData_SteamAudio : public Audio::IATLAudioObjectData
    {
        AZ_DEFAULT_COPY_MOVE(SATLAudioObjectData_SteamAudio); // NOLINT

        SATLAudioObjectData_SteamAudio(const SA_GameObjectId id, const bool hasPosition)
            : m_hasPosition(hasPosition)
            , m_id(id)
        {
        }

        ~SATLAudioObjectData_SteamAudio() override = default;

        bool m_hasPosition;
        SA_GameObjectId m_id;
    };

    struct SATLTriggerImplData_SteamAudio : public Audio::IATLTriggerImplData
    {
        explicit SATLTriggerImplData_SteamAudio(SA_UniqueId uniqueId)
            : m_uniqueId{ uniqueId } {};

        SA_UniqueId const m_uniqueId;
    };

    struct SATLListenerData_SteamAudio : public Audio::IATLListenerData
    {
        explicit SATLListenerData_SteamAudio(SA_GameObjectId const id)
            : m_listenerObjectId(id)
        {
        }

        SA_GameObjectId m_listenerObjectId = SA_GameObjectId();
    };

    struct SATLRtpcImplData_SteamAudio : public Audio::IATLRtpcImplData
    {
        SATLRtpcImplData_SteamAudio(const SA_RtpcId id, const float passedMult, const float passedShift)
            : m_mult(passedMult)
            , m_shift(passedShift)
            , m_id(id)
        {
        }

        float m_mult;
        float m_shift;
        SA_RtpcId m_id;
    };

    struct SATLEventData_SteamAudio : public Audio::IATLEventData
    {
        explicit SATLEventData_SteamAudio(const Audio::TAudioEventID passedId)
            : m_audioEventState(Audio::eAES_NONE)
            , m_maId(AZ::Crc32())
            , m_atlId(passedId)
            , m_sourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        Audio::EAudioEventState m_audioEventState;
        SA_UniqueId m_maId;
        const Audio::TAudioEventID m_atlId;
        Audio::TAudioSourceId m_sourceId;
    };

    struct SATLAudioFileEntryData_SteamAudio : public Audio::IATLAudioFileEntryData
    {
        SATLAudioFileEntryData_SteamAudio()
            : m_bankId(AZ::Crc32())
        {
        }

        ~SATLAudioFileEntryData_SteamAudio() override = default;

        SA_LibraryId m_bankId;
    };

} // namespace SteamAudio
