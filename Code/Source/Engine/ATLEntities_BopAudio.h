#pragma once

#include <ATLEntityData.h>
#include <AudioAllocators.h>
#include <AzCore/Math/Crc.h>
#include <IAudioInterfacesCommonData.h>

namespace BopAudio
{

    using BA_GameObjectId = AZ::Crc32;
    using BA_RtpcId = AZ::Crc32;
    using BA_UniqueId = AZ::Crc32;
    using BA_LibraryId = AZ::Crc32;

    using UniqueIDVector = AZStd::vector<BA_UniqueId, Audio::AudioImplStdAllocator>;

    struct SATLAudioObjectData_BopAudio : public Audio::IATLAudioObjectData
    {
        SATLAudioObjectData_BopAudio(BA_GameObjectId const id, bool const hasPosition)
            : m_hasPosition(hasPosition)
            , m_id(id)
        {
        }

        ~SATLAudioObjectData_BopAudio() override = default;

        bool m_hasPosition;
        BA_GameObjectId m_id;
    };

    struct SATLTriggerImplData_BopAudio : public Audio::IATLTriggerImplData
    {
        explicit SATLTriggerImplData_BopAudio(BA_UniqueId uniqueId)
            : m_uniqueId{ uniqueId } {};

        BA_UniqueId const m_uniqueId;
    };

    struct SATLListenerData_BopAudio : public Audio::IATLListenerData
    {
        explicit SATLListenerData_BopAudio(BA_GameObjectId const id)
            : m_listenerObjectId(id)
        {
        }

        BA_GameObjectId m_listenerObjectId = BA_GameObjectId();
    };

    struct SATLRtpcImplData_BopAudio : public Audio::IATLRtpcImplData
    {
        SATLRtpcImplData_BopAudio(BA_RtpcId const id, float const passedMult, float const passedShift)
            : m_mult(passedMult)
            , m_shift(passedShift)
            , m_id(id)
        {
        }

        float m_mult;
        float m_shift;
        BA_RtpcId m_id;
    };

    struct SATLEventData_BopAudio : public Audio::IATLEventData
    {
        explicit SATLEventData_BopAudio(Audio::TAudioEventID const passedId)
            : m_audioEventState(Audio::eAES_NONE)
            , m_maId(AZ::Crc32())
            , m_atlId(passedId)
            , m_sourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        ~SATLEventData_BopAudio() override = default;

        Audio::EAudioEventState m_audioEventState;
        BA_UniqueId m_maId;
        Audio::TAudioEventID m_atlId;
        Audio::TAudioSourceId m_sourceId;
    };

    struct SATLAudioFileEntryData_BopAudio : public Audio::IATLAudioFileEntryData
    {
        SATLAudioFileEntryData_BopAudio()
            : m_bankId(AZ::Crc32())
        {
        }

        ~SATLAudioFileEntryData_BopAudio() override = default;

        BA_LibraryId m_bankId;
    };

} // namespace BopAudio
