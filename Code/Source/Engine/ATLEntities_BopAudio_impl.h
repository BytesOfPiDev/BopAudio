#pragma once

#include "Engine/ATLEntities_BopAudio.h"

#if defined(USE_MINIAUDIO)
#include "MiniAudio/MiniAudioConstants.h"
#endif

namespace BopAudio
{
    class SATLAudioObjectData_bopaudio : public IATLAudioObjectData_bopaudio
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SATLAudioObjectData_bopaudio);

        SATLAudioObjectData_bopaudio() = default;

        SATLAudioObjectData_bopaudio(Audio::TAudioObjectID atlAudioObjectId)
            : m_atlAudioObjectId(atlAudioObjectId){};

        ~SATLAudioObjectData_bopaudio() override = default;

        [[nodiscard]] auto GetAtlAudioObjectId() const -> Audio::TAudioObjectID override
        {
            return m_atlAudioObjectId;
        }

        void ChangeName(AZStd::string_view objectName) override
        {
            m_objectName = objectName;
        }

        [[nodiscard]] auto GetName() const -> AZ::Name override
        {
            return m_objectName;
        }

    private:
        Audio::TAudioObjectID m_atlAudioObjectId;
        AZ::Name m_objectName;
    };

    struct SATLTriggerImplData_bopaudio : public IATLTriggerImplData_bopaudio
    {
        SATLTriggerImplData_bopaudio() = default;
        SATLTriggerImplData_bopaudio(Audio::TAudioEventID audioEventId)
            : m_audioEventId(audioEventId){};

        [[nodiscard]] auto GetEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

    private:
        Audio::TAudioEventID m_audioEventId{};
    };

    struct SATLListenerData_bopaudio : public IATLListenerData_script
    {
        explicit SATLListenerData_bopaudio(Audio::TAudioObjectID id)
            : m_listenerObjectId(id)
        {
#if USE_MINIAUDIO
            m_entity.emplace("script_audio_listener");
            m_entity->CreateComponent(AZ::TypeId{ MiniAudio::MiniAudioListenerComponentTypeId });

            m_entity->Init();
            m_entity->Activate();
#endif
        }

        Audio::TAudioObjectID m_listenerObjectId{};
        AZStd::optional<AZ::Entity> m_entity;
    };

    class SATLEventData_bopaudio : public Audio::IATLEventData
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SATLEventData_bopaudio);

        SATLEventData_bopaudio() = default;
        explicit SATLEventData_bopaudio(Audio::TAudioEventID eventId)
            : m_eventId{ eventId } {};

        ~SATLEventData_bopaudio() override = default;

        [[nodiscard]] auto GetAtlEventId() const -> Audio::TAudioEventID
        {
            return m_eventId;
        }

    private:
        Audio::TAudioEventID m_eventId{};
    };

    struct SATLAudioFileEntryData_script : public Audio::IATLAudioFileEntryData
    {
        AZ_DEFAULT_COPY_MOVE(SATLAudioFileEntryData_script);

        SATLAudioFileEntryData_script() = default;

        ~SATLAudioFileEntryData_script() override{};
    };

} // namespace BopAudio
