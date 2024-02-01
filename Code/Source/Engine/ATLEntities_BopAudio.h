#pragma once

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Math/Crc.h"
#include "Engine/Sound.h"
#include "IAudioInterfacesCommonData.h"

#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    static constexpr auto MaxStringIdSize{ 32 };

    using BA_GameObjectId = Audio::TAudioObjectID;
    using BA_RtpcId = AZ::Crc32;
    using BA_UniqueId = AZ::Crc32;
    constexpr auto InvalidBaUniqueId{ AZ::Crc32{} };
    using BA_TriggerId = AZ::Crc32;
    using BA_SoundId = AZ::Crc32;
    using BA_SoundBankId = AZ::Name;
    using BA_Name = AZStd::string;

    using UniqueIDVector = AZStd::vector<BA_UniqueId, Audio::AudioImplStdAllocator>;

    struct SATLAudioObjectData_BopAudio : public Audio::IATLAudioObjectData
    {
        AZ_DISABLE_COPY_MOVE(SATLAudioObjectData_BopAudio);

        SATLAudioObjectData_BopAudio(BA_GameObjectId id)
            : m_id(id)
            , m_name{}
            , m_transform{}
            , m_sound{}
        {
        }

        ~SATLAudioObjectData_BopAudio() override = default;

        BA_GameObjectId m_id;
        AZ::Name m_name;
        AZ::Transform m_transform;
        SoundPtr m_sound;
    };

    struct SATLTriggerImplData_BopAudio : public Audio::IATLTriggerImplData
    {
        explicit SATLTriggerImplData_BopAudio(BA_UniqueId id)
            : m_id{ id }
        {
        }

        BA_UniqueId m_id;
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
            , m_baId{}
            , m_atlId(passedId)
            , m_sourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        ~SATLEventData_BopAudio() override = default;

        Audio::EAudioEventState m_audioEventState;
        BA_UniqueId m_baId;
        Audio::TAudioEventID m_atlId;
        Audio::TAudioSourceId m_sourceId;
    };

    struct SATLAudioFileEntryData_BopAudio : public Audio::IATLAudioFileEntryData
    {
        SATLAudioFileEntryData_BopAudio()
            : m_bankId(AZ::Crc32())
        {
        }

        ~SATLAudioFileEntryData_BopAudio() override
        {
            m_sounds.clear();
            m_soundAssets.clear();
            m_soundNames.clear();
        };

        BA_SoundBankId m_bankId;
        AZStd::unordered_set<AZ::Name> m_soundNames;
        AZStd::unordered_map<AZ::Name, MiniAudio::SoundDataAsset> m_soundAssets{};
        AZStd::unordered_map<AZ::Name, SoundPtr> m_sounds{};
    };

} // namespace BopAudio
