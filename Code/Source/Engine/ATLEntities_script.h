#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio 
{

    class SATLAudioObjectData_script : public Audio::IATLAudioObjectData
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SATLAudioObjectData_script);

        SATLAudioObjectData_script(
            Audio::TAudioObjectID atlAudioObjectId, AZStd::string_view objectName = {})
            : m_atlAudioObjectId(atlAudioObjectId)
            , m_objectName{ objectName }
        {
        }

        ~SATLAudioObjectData_script() override = default;

        [[nodiscard]] constexpr auto GetAtlAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_atlAudioObjectId;
        }

        void SetAtlAudioObjectId(Audio::TAudioObjectID) = delete;

        void ChangeName(AZStd::string_view objectName)
        {
            m_objectName = objectName;
        }

    private:
        Audio::TAudioObjectID m_atlAudioObjectId;
        AZ::Name m_objectName;
    };

    struct SATLTriggerImplData_script : public Audio::IATLTriggerImplData
    {
        SATLTriggerImplData_script() = default;
        SATLTriggerImplData_script(Audio::TAudioEventID audioEventId)
            : m_audioEventId(audioEventId){};

        [[nodiscard]] auto GetEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

    private:
        Audio::TAudioEventID m_audioEventId{};
    };

    struct SATLListenerData_script : public Audio::IATLListenerData
    {
        explicit SATLListenerData_script(Audio::TAudioObjectID id)
            : m_listenerObjectId(id)
        {
        }

        Audio::TAudioObjectID m_listenerObjectId{};
    };

    class SATLEventData_script : public Audio::IATLEventData
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SATLEventData_script);

        explicit SATLEventData_script(Audio::TAudioEventID const atlEventId)
            : m_atlEventId(atlEventId)
        {
        }

        ~SATLEventData_script() override = default;

        [[nodiscard]] constexpr auto GetAtlEventId() const -> Audio::TAudioEventID
        {
            return m_atlEventId;
        }

        constexpr void SetAtlEventId(Audio::TAudioEventID atlEventId)
        {
            m_atlEventId = atlEventId;
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
        Audio::TAudioEventID m_atlEventId{};
        Audio::TAudioSourceId m_sourceId{};
    };

    struct SATLAudioFileEntryData_script : public Audio::IATLAudioFileEntryData
    {
        AZ_DEFAULT_COPY_MOVE(SATLAudioFileEntryData_script);

        SATLAudioFileEntryData_script() = default;

        ~SATLAudioFileEntryData_script() override{};
    };

} // namespace script
