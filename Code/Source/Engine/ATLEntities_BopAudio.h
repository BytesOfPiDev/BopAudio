#pragma once

#include "ATLEntityData.h"
#include "AzCore/Component/Entity.h"
#include "AzCore/Name/Name.h"
#include "IAudioInterfacesCommonData.h"
#include "MiniAudio/MiniAudioConstants.h"

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

    class SATLEventData_script : public Audio::IATLEventData
    {
    public:
        AZ_DEFAULT_COPY_MOVE(SATLEventData_script);

        explicit SATLEventData_script(Audio::TAudioEventID eventId)
            : m_eventId{ eventId } {};

        ~SATLEventData_script() override = default;

        Audio::TAudioEventID m_eventId{};
    };

    struct SATLAudioFileEntryData_script : public Audio::IATLAudioFileEntryData
    {
        AZ_DEFAULT_COPY_MOVE(SATLAudioFileEntryData_script);

        SATLAudioFileEntryData_script() = default;

        ~SATLAudioFileEntryData_script() override{};
    };

} // namespace BopAudio
