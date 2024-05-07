#include "Engine/AudioSystemImpl_BopAudio.h"

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Component/ComponentApplicationBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "AzCore/Time/ITime.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/ATLEntities_BopAudio_impl.h"
#include "Engine/Common_BopAudio.h"

namespace BopAudio
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        AudioSystemImpl_bopaudio,
        "AudioSystemImpl_bopaudio",
        "{4BDD8703-0408-4196-AF77-8E22803D3974}");

    char const* const AudioSystemImpl_bopaudio::BopAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_bopaudio::AudioSystemImpl_bopaudio()
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);

        SetPaths();

        Audio::AudioSystemImplementationRequestBus::Handler::BusConnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusConnect();
    }

    AudioSystemImpl_bopaudio::~AudioSystemImpl_bopaudio()
    {
        Audio::AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AudioSystemImpl_bopaudio::SetPaths()
    {
    }

    void AudioSystemImpl_bopaudio::OnAudioSystemLoseFocus() {};

    void AudioSystemImpl_bopaudio::OnAudioSystemGetFocus() {};

    void AudioSystemImpl_bopaudio::OnAudioSystemMuteAll() {};

    void AudioSystemImpl_bopaudio::OnAudioSystemUnmuteAll() {};

    void AudioSystemImpl_bopaudio::OnAudioSystemRefresh() {};

    void AudioSystemImpl_bopaudio::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_bopaudio::Initialize() -> Audio::EAudioRequestStatus
    {
        AZ_Info(TYPEINFO_Name(), "AudioSystemImplementation nitialized.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        AZ_Info(TYPEINFO_Name(), "AudioSystemImplementation shutting down.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::Release() -> Audio::EAudioRequestStatus
    {
        AZ_Info(TYPEINFO_Name(), "AudioSystemImplementation releasing.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZ_Info(TYPEINFO_Name(), "Stopping all sounds.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::RegisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData,
        char const* const objectName) -> Audio::EAudioRequestStatus
    {
        AZLOG(
            LOG_asi_bopaudio,
            "Attempting to register an audio object: [objectName: %s.].\n",
            objectName);

        if (audioObjectData == nullptr)
        {
            AZ_Error(TYPEINFO_Name(), false, "Unable to register audio object with nullptr.\n");
            return Audio::EAudioRequestStatus::Failure;
        }

        auto* bopAudioObject{ static_cast<IATLAudioObjectData_bopaudio*>(audioObjectData) };
        bopAudioObject->ChangeName(objectName);

        m_registeredObjects.insert(audioObjectData);

        AZ_Error(
            TYPEINFO_Name(),
            m_registeredObjects.contains(audioObjectData),
            "Something went wrong while inserting the object.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::UnregisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        if (!audioObjectData)
        {
            return Audio::EAudioRequestStatus::FailureInvalidRequest;
        }

        auto const numErased{ m_registeredObjects.erase(audioObjectData) };

        AZ_Error(TYPEINFO_Name(), numErased, "Something went wrong while erasing the object.\n");

        return numErased > 0 ? Audio::EAudioRequestStatus::Success
                             : Audio::EAudioRequestStatus::FailureInvalidRequest;
    }

    auto AudioSystemImpl_bopaudio::ResetAudioObject(
        Audio::IATLAudioObjectData* const /*audioObjectData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Resetting audio object.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::UpdateAudioObject(
        Audio::IATLAudioObjectData* const /*audioObjectData*/) -> Audio::EAudioRequestStatus
    {
        static AZStd::atomic<AZ::TimeUs> lastLogTime{};
        if (AZ::GetSimulationTickDeltaTimeUs() - lastLogTime > AZ::TimeUs{ AZ::SecondsToTimeUs(5) })
        {
            lastLogTime = AZ::GetSimulationTickDeltaTimeUs();
            AZLOG(LOG_asi_bopaudio, "[5SecInt] Updating audio object\n");
        }
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLTriggerImplData const* const /*triggerData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Preparing trigger sync.\n");
        return Audio::EAudioRequestStatus::Success;
    }
    auto AudioSystemImpl_bopaudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Unpreparing trigger sync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLTriggerImplData const* const /*triggerData*/,
        Audio::IATLEventData* const /*eventData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Preparing trigger async.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLTriggerImplData const* const /*triggerData*/,
        Audio::IATLEventData* const /*eventData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Unpreparing trigger async.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData,
        Audio::SATLSourceData const* const /*pSourceData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Activating audio trigger.\n");

        [[maybe_unused]] auto* const implTriggerData{
            static_cast<SATLTriggerImplData_bopaudio const*>(triggerData)
        };

        auto* const implAudioObjectData{ static_cast<SATLAudioObjectData_bopaudio*>(
            audioObjectData) };

        auto* const implEventData{ static_cast<SATLEventData_bopaudio*>(eventData) };

        if (!implAudioObjectData)
        {
            AZ_Error(
                TYPEINFO_Name(),
                false,
                "Unable to activate trigger because the object data is null.\n");

            return Audio::EAudioRequestStatus::FailureInvalidRequest;
        }

        if (!implEventData)
        {
            AZ_Error(
                TYPEINFO_Name(),
                false,
                "Unable to activate trigger because the event data is null.\n");

            return Audio::EAudioRequestStatus::Failure;
        }

        if (implAudioObjectData->GetAtlAudioObjectId() == INVALID_AUDIO_OBJECT_ID)
        {
            AZ_Error(
                TYPEINFO_Name(),
                false,
                "Unable to activate trigger because the object id is not valid.\n");

            return Audio::EAudioRequestStatus::FailureInvalidControlId;
        }

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::StopEvent(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLEventData const* const /*eventData*/) -> Audio::EAudioRequestStatus
    {
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::StopAllEvents(
        Audio::IATLAudioObjectData* const /*audioObjectData*/) -> Audio::EAudioRequestStatus
    {
        AZ_Error(TYPEINFO_Name(), false, "StopAllEvents is not yet implemented.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetPosition(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::SATLWorldPosition const& /*worldPosition*/) -> Audio::EAudioRequestStatus
    {
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::MultiPositionParams const& /*multiPositionParams*/) -> Audio::EAudioRequestStatus
    {
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetEnvironment(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLEnvironmentImplData const* const /*environmentData*/,
        float const /*amount*/) -> Audio::EAudioRequestStatus
    {
        AZ_Error(TYPEINFO_Name(), false, "Setting audio environment.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetRtpc(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLRtpcImplData const* const /*rtpcData*/,
        float const /*value*/) -> Audio::EAudioRequestStatus
    {
        AZ_Warning(
            TYPEINFO_Name(), false, "Unable to set Rtpc because it is not yet implemented.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetSwitchState(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLSwitchStateImplData const* const /*switchStateData*/)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Setting audio switch state.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData,
        float const obstruction,
        float const occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData,
        Audio::SATLWorldPosition const& newPosition) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::RegisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZ_Error(TYPEINFO_Name(), false, "Registering in-memory audio file.\n");

        if (!audioFileEntry)
        {
            AZ_Error(
                TYPEINFO_Name(),
                false,
                "Unable to register an in-memory file with null file entry.\n");

            return Audio::EAudioRequestStatus::Failure;
        }

        AZ_Error(TYPEINFO_Name(), false, "Registering an in-memory file is not implemented.\n");

        return Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_bopaudio::UnregisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const /*audioFileEntry*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_asi_bopaudio, "Unregistering in-memory audio file.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_bopaudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* /*audioFileEntryNode*/,
        Audio::SATLAudioFileEntryInfo* const /*fileEntryInfo*/) -> Audio::EAudioRequestStatus
    {
        AZ_Error(
            TYPEINFO_Name(),
            false,
            "Unable to parse audio file entry because it is not implemented.\n");
        return Audio::EAudioRequestStatus::Failure;
    }

    void AudioSystemImpl_bopaudio::DeleteAudioFileEntryData(
        Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZLOG(LOG_asi_bopaudio, "Deleting audio file entry data.\n");
        azdestroy(oldAudioFileEntryData, Audio::AudioImplAllocator, SATLAudioFileEntryData_script);
    }

    auto AudioSystemImpl_bopaudio::GetAudioFileLocation(
        Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> char const* const
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetAudioFileLocation.");

        if (!fileEntryInfo)
        {
            return nullptr;
        }

        return fileEntryInfo->bLocalized ? m_localizedSoundBankFolder.c_str()
                                         : m_soundBankFolder.c_str();
    }

    auto AudioSystemImpl_bopaudio::NewAudioTriggerImplData(
        const AZ::rapidxml::xml_node<char>* audioTriggerNode) -> Audio::IATLTriggerImplData*
    {
        if (!audioTriggerNode ||
            !AZ::StringFunc::Equal(audioTriggerNode->name(), XmlTags::TriggerTag))
        {
            AZ_Error(
                TYPEINFO_Name(),
                false,
                "Unable to create a new trigger. The XML node is not valid.");

            return nullptr;
        }

        AZStd::string const triggerName = [&audioTriggerNode]() -> decltype(triggerName)
        {
            auto* const triggerNameAttrib{ audioTriggerNode->first_attribute(
                XmlTags::NameAttribute) };

            return decltype(triggerName){ triggerNameAttrib ? triggerNameAttrib->value() : "" };
        }();

        if (triggerName.empty())
        {
            AZ_Error(TYPEINFO_Name(), false, "Failed to find the audio trigger's name.\n");
            return nullptr;
        }

        auto* implAudioTriggerData{ azcreate(
            SATLTriggerImplData_bopaudio,
            (Audio::AudioStringToID<Audio::TAudioEventID>(triggerName.c_str())),
            Audio::AudioImplAllocator) };

        AZLOG(
            LOG_asi_bopaudio,
            "ASI created a new audio trigger implementation data object. [Trigger: %s | "
            "AudioEventId: %llu. NativeId: %llu\n",
            triggerName.c_str(),
            static_cast<AZ::u64>(implAudioTriggerData->GetEventId()),
            Audio::AudioStringToID<Audio::TAudioEventID>(triggerName.c_str()));

        return implAudioTriggerData;
    }

    void AudioSystemImpl_bopaudio::DeleteAudioTriggerImplData(
        Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZLOG(LOG_asi_bopaudio, "Deleting audio trigger data.\n");
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, SATLTriggerImplData_bopaudio);
    }

    auto AudioSystemImpl_bopaudio::NewAudioRtpcImplData(
        const AZ::rapidxml::xml_node<char>* /*audioRtpcNode*/) -> Audio::IATLRtpcImplData*
    {
        AZ_Error(TYPEINFO_Name(), false, "NewAudioRtpcImplData is not yet implemented.\n");
        return nullptr;
    }

    void AudioSystemImpl_bopaudio::DeleteAudioRtpcImplData(
        Audio::IATLRtpcImplData* const /*oldRtpcImplData*/)
    {
        AZ_Error(TYPEINFO_Name(), false, "DeleteAudioRtpcImplData is not yet implemented.\n");
    }

    auto AudioSystemImpl_bopaudio::NewAudioSwitchStateImplData(
        const AZ::rapidxml::xml_node<char>* /*audioSwitchStateNode*/)
        -> Audio::IATLSwitchStateImplData*
    {
        AZ_Error(TYPEINFO_Name(), false, "NewAudioSwitchStateImplData is not yet implemented.");
        return nullptr;
    }

    void AudioSystemImpl_bopaudio::DeleteAudioSwitchStateImplData(
        Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZLOG_INFO("ASI: DeleteAudioSwitchStateImplData.");
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_bopaudio::NewAudioEnvironmentImplData(
        const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) -> Audio::IATLEnvironmentImplData*
    {
        AZLOG(
            ASI_BopAudio,
            "BopAudio: NewAudioEnvironmentImplData. [audioEnvironmentNode: "
            "%s].",
            audioEnvironmentNode->name());
        return nullptr;
    }

    void AudioSystemImpl_bopaudio::DeleteAudioEnvironmentImplData(
        Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_bopaudio::NewGlobalAudioObjectData(Audio::TAudioObjectID const atlObjectId)
        -> Audio::IATLAudioObjectData*
    {
        AZLOG_INFO("ASI: NewGlobalAudioObjectData. [objectId: %llu].", atlObjectId);

        return azcreate(SATLAudioObjectData_bopaudio, (atlObjectId), Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_bopaudio::NewAudioObjectData(Audio::TAudioObjectID const atlObjectId)
        -> Audio::IATLAudioObjectData*
    {
        return azcreate(SATLAudioObjectData_bopaudio, (atlObjectId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_bopaudio::DeleteAudioObjectData(
        Audio::IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData, Audio::AudioImplAllocator, SATLAudioObjectData_bopaudio);
    }

    auto AudioSystemImpl_bopaudio::NewDefaultAudioListenerObjectData(
        [[maybe_unused]] Audio::TATLIDType const objectId) -> Audio::IATLListenerData*
    {
        AZLOG(LOG_asi_bopaudio, "Creating new default audio listener object data.");
#if USE_MINIAUDIO
        auto* newListenerObject{ azcreate(
            SATLListenerData_bopaudio, (objectId), Audio::AudioImplAllocator) };
        return newListenerObject;
#else
        return {};
#endif
    }

    auto AudioSystemImpl_bopaudio::NewAudioListenerObjectData(
        [[maybe_unused]] Audio::TATLIDType const objectId) -> Audio::IATLListenerData*
    {
#if USE_MINIAUDIO
        auto* newListenerObject{ azcreate(
            SATLListenerData_bopaudio, (objectId), Audio::AudioImplAllocator) };
        return newListenerObject;
#else
        return {};
#endif
    }

    void AudioSystemImpl_bopaudio::DeleteAudioListenerObjectData(
        [[maybe_unused]] Audio::IATLListenerData* const oldListenerData)
    {
        AZLOG(LOG_asi_bopaudio, "Deleting audio listener object data.\n");
        azdestroy(oldListenerData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_bopaudio::NewAudioEventData(Audio::TAudioEventID const eventId)
        -> Audio::IATLEventData*
    {
        AZLOG(LOG_asi_bopaudio, "Creating new audio event data.\n");
        return azcreate(SATLEventData_bopaudio, (eventId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_bopaudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioEventData.");
        azdestroy(oldEventData, Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_bopaudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZLOG(LOG_asi_bopaudio, "BopAudio: ResetAudioEventData.");

        if (!eventData)
        {
            AZ_Error(TYPEINFO_Name(), false, "Received nullptr. Valid address required.");

            return;
        }
    }

    auto AudioSystemImpl_bopaudio::GetImplSubPath() const -> char const* const
    {
        return BopAudioImplSubPath;
    }

    void AudioSystemImpl_bopaudio::SetLanguage(char const* const language)
    {
        m_language = language;
    }

    auto AudioSystemImpl_bopaudio::GetImplementationNameString() const -> char const* const
    {
        return ImplName;
    }

    void AudioSystemImpl_bopaudio::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZLOG(LOG_asi_bopaudio, "BopAudio: GetMemoryInfo.");
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_bopaudio::GetMemoryPoolInfo()
        -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        AZLOG(LOG_asi_bopaudio, "BopAudio: GetMemoryInfo.");
        return {};
    }

    auto AudioSystemImpl_bopaudio::CreateAudioSource(
        Audio::SAudioInputConfig const& /*sourceConfig*/) -> bool
    {
        AZ_Error(TYPEINFO_Name(), false, "Creating an audio source is not supported\n");
        return true;
    }

    void AudioSystemImpl_bopaudio::DestroyAudioSource(Audio::TAudioSourceId /*sourceId*/)
    {
    }

    void AudioSystemImpl_bopaudio::SetPanningMode(Audio::PanningMode mode)
    {
        m_panningMode = mode;
    }
} // namespace BopAudio
