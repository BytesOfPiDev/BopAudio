#include "Engine/AudioSystemImpl_script.h"

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Clients/ParameterTypes.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "AzCore/Component/ComponentApplicationBus.h"

#include "BopAudio/BopAudioBus.h"
#include "Engine/ATLEntities_script.h"
#include "Engine/Common_script.h"

namespace BopAudio
{
    char const* const AudioSystemImpl_script::ScriptAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_script::AudioSystemImpl_script()
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);

        SetPaths();

        Audio::AudioSystemImplementationRequestBus::Handler::BusConnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusConnect();

        AsiInterface::Register(this);
    }

    AudioSystemImpl_script::~AudioSystemImpl_script()
    {
        AsiInterface::Unregister(this);

        Audio::AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AudioSystemImpl_script::SetPaths()
    {
    }

    void AudioSystemImpl_script::OnAudioSystemLoseFocus()
    {
        AZLOG_INFO("Bop Audio lost focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_script::OnAudioSystemGetFocus()
    {
        AZLOG_INFO("Bop Audio has focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_script::OnAudioSystemMuteAll()
    {
        AZLOG_INFO("Bop Audio received MuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_script::OnAudioSystemUnmuteAll()
    {
        AZLOG_INFO("Bop Audio received UnmuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_script::OnAudioSystemRefresh()
    {
        AZLOG_INFO("Bop Audio received Refresh. Unimplemented.\n");
    }

    void AudioSystemImpl_script::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_script::Initialize() -> Audio::EAudioRequestStatus
    {
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::ShutDown() -> Audio::EAudioRequestStatus
    {
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::Release() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received Release.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received StopAllSounds.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::RegisterAudioObject(
        Audio::IATLAudioObjectData* const /*audioObjectData*/, char const* const objectName)
        -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: RegisterAudioObject. [objectName: %s.].", objectName);

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::UnregisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: UnRegisterAudioObject");

        if (!audioObjectData)
        {
            AZLOG_ERROR("BopAudio: UnRegisterAudioObject failed.");
            return Audio::EAudioRequestStatus::Failure;
        }

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::UpdateAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const /*triggerData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }
    auto AudioSystemImpl_script::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData,
        Audio::SATLSourceData const* const /*pSourceData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_ASI, "ASI received ActivateTrigger");

        [[maybe_unused]] auto* const implTriggerData{
            static_cast<SATLTriggerImplData_script const*>(triggerData)
        };
        auto* const implAudioObjectData{ static_cast<SATLAudioObjectData_script*>(
            audioObjectData) };
        auto* const implEventData{ static_cast<SATLEventData_script*>(eventData) };

        if (!implAudioObjectData)
        {
            AZ_Error("ASI", false, "Received nullptr for the object data!");
            return Audio::EAudioRequestStatus::FailureInvalidRequest;
        }

        if (!implEventData)
        {
            AZ_Error("ASI", false, "Received nullptr for the event data!");
            return Audio::EAudioRequestStatus::Failure;
        }

        if (implAudioObjectData->GetAtlAudioObjectId() == INVALID_AUDIO_OBJECT_ID)
        {
            AZ_Error("ASI", false, "Invalid control id.");
            return Audio::EAudioRequestStatus::FailureInvalidControlId;
        }

        StartEventData startEventData{};
        startEventData.m_owner = eventData->m_owner;

        return Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_script::StopEvent(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLEventData const* const eventData) -> Audio::EAudioRequestStatus
    {
        [[maybe_unused]] auto const* const implEventData{ static_cast<SATLEventData_script const*>(
            eventData) };

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData)
        -> Audio::EAudioRequestStatus
    {
        AZ_Error("ASI", false, "StopAllEvents is not yet implemented.");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_script::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] Audio::SATLWorldPosition const& worldPosition)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetPosition.\n");
        [[maybe_unused]] auto bopAudioObjectData{ static_cast<SATLAudioObjectData_script*>(
            audioObjectData) };

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::MultiPositionParams const& multiPositionParams) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLEnvironmentImplData const* const environmentData,
        float const amount) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData,
        float const value) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLSwitchStateImplData const* const switchStateData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData,
        float const obstruction,
        float const occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, Audio::SATLWorldPosition const& newPosition)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::RegisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: RegisterInMemoryFile.\n");

        if (!audioFileEntry)
        {
            AZ_Error(
                "AudioSystemImpl_script",
                false,
                "Unable to register an in-memory file with null file entry.\n");

            return Audio::EAudioRequestStatus::Failure;
        }

        AZ_Error(
            "AudioSystemImpl_script", false, "Registering an in-memory file is not implemented.\n");
        return Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_script::UnregisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnregisterInMemoryFile.");
        AZ_UNUSED(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_script::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* /*audioFileEntryNode*/,
        Audio::SATLAudioFileEntryInfo* const /*fileEntryInfo*/) -> Audio::EAudioRequestStatus
    {
        AZ_Warning(
            "AudioSystemImpl",
            false,
            "Unable to parse audio file entry because it is not implemented.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    void AudioSystemImpl_script::DeleteAudioFileEntryData(
        Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioFileEntryData.");
        azdestroy(oldAudioFileEntryData, Audio::AudioImplAllocator, SATLAudioFileEntryData_script);
    }

    auto AudioSystemImpl_script::GetAudioFileLocation(
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

    auto AudioSystemImpl_script::NewAudioTriggerImplData(
        const AZ::rapidxml::xml_node<char>* audioTriggerNode) -> Audio::IATLTriggerImplData*
    {
        if (!audioTriggerNode ||
            !AZ::StringFunc::Equal(audioTriggerNode->name(), XmlTags::TriggerTag))
        {
            return nullptr;
        }

        AZStd::string const triggerName = [&audioTriggerNode]() -> decltype(triggerName)
        {
            auto const triggerNameAttrib{ audioTriggerNode->first_attribute(
                XmlTags::NameAttribute) };
            return decltype(triggerName){ triggerNameAttrib ? triggerNameAttrib->value()
                                                            : nullptr };
        }();

        auto* implAudioTriggerData{ azcreate(
            SATLTriggerImplData_script,
            (Audio::AudioStringToID<Audio::TAudioEventID>(triggerName.c_str())),
            Audio::AudioImplAllocator) };

        AZLOG(
            LOG_ASI,
            "ASI created a new audio trigger implementation data object. [Trigger: %s | "
            "AudioEventId: %llu. NativeId: %llu\n",
            triggerName.c_str(),
            static_cast<AZ::u64>(implAudioTriggerData->GetEventId()),
            Audio::AudioStringToID<Audio::TAudioEventID>(triggerName.c_str()));

        return implAudioTriggerData;
    }

    void AudioSystemImpl_script::DeleteAudioTriggerImplData(
        Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZLOG(LOG_ASI, "ASI: DeleteAudioTriggerImplData called");
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, SATLTriggerImplData_script);
    }

    auto AudioSystemImpl_script::NewAudioRtpcImplData(
        const AZ::rapidxml::xml_node<char>* /*audioRtpcNode*/) -> Audio::IATLRtpcImplData*
    {
        AZ_Error("ASI", false, "NewAudioRtpcImplData is not yet implemented");
        return nullptr;
    }

    void AudioSystemImpl_script::DeleteAudioRtpcImplData(
        Audio::IATLRtpcImplData* const /*oldRtpcImplData*/)
    {
        AZ_Error("ASI", false, "DeleteAudioRtpcImplData is not yet implemented");
    }

    auto AudioSystemImpl_script::NewAudioSwitchStateImplData(
        const AZ::rapidxml::xml_node<char>* /*audioSwitchStateNode*/)
        -> Audio::IATLSwitchStateImplData*
    {
        AZ_Error(
            "AudioSystemImpl_script", false, "NewAudioSwitchStateImplData is not yet implemented.");
        return nullptr;
    }

    void AudioSystemImpl_script::DeleteAudioSwitchStateImplData(
        Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZLOG_INFO("ASI: DeleteAudioSwitchStateImplData.");
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_script::NewAudioEnvironmentImplData(
        const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) -> Audio::IATLEnvironmentImplData*
    {
        AZLOG(
            ASI_BopAudio,
            "BopAudio: NewAudioEnvironmentImplData. [audioEnvironmentNode: "
            "%s].",
            audioEnvironmentNode->name());
        return nullptr;
    }

    void AudioSystemImpl_script::DeleteAudioEnvironmentImplData(
        Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_script::NewGlobalAudioObjectData(Audio::TAudioObjectID const atlObjectId)
        -> Audio::IATLAudioObjectData*
    {
        AZLOG_INFO("ASI: NewGlobalAudioObjectData. [objectId: %llu].", atlObjectId);

        return azcreate(SATLAudioObjectData_script, (atlObjectId), Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_script::NewAudioObjectData(Audio::TAudioObjectID const atlObjectId)
        -> Audio::IATLAudioObjectData*
    {
        return azcreate(SATLAudioObjectData_script, (atlObjectId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_script::DeleteAudioObjectData(
        Audio::IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData, Audio::AudioImplAllocator, SATLAudioObjectData_script);
    }

    auto AudioSystemImpl_script::NewDefaultAudioListenerObjectData(Audio::TATLIDType const objectId)
        -> Audio::IATLListenerData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewDefaultAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_script::NewAudioListenerObjectData(Audio::TATLIDType const objectId)
        -> Audio::IATLListenerData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    void AudioSystemImpl_script::DeleteAudioListenerObjectData(
        Audio::IATLListenerData* const oldListenerData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioListenerObjectData.");
        AZ_UNUSED(oldListenerData);
    }

    auto AudioSystemImpl_script::NewAudioEventData(Audio::TAudioEventID const eventId)
        -> Audio::IATLEventData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioEventData");
        return azcreate(SATLEventData_script, (eventId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_script::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioEventData.");
        azdestroy(oldEventData, Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_script::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioEventData.");

        if (!eventData)
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio", false, "Received nullptr. Valid address required.");
            return;
        }

        auto* const bopEventData{ static_cast<SATLEventData_script*>(eventData) };

        bopEventData->SetSourceId(INVALID_AUDIO_SOURCE_ID);
    }

    auto AudioSystemImpl_script::GetImplSubPath() const -> char const* const
    {
        return ScriptAudioImplSubPath;
    }

    void AudioSystemImpl_script::SetLanguage(char const* const language)
    {
        m_language = language;
    }

    // Functions below are only used when RELEASE is not defined
    auto AudioSystemImpl_script::GetImplementationNameString() const -> char const* const
    {
        return ImplName;
    }

    void AudioSystemImpl_script::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_script::GetMemoryPoolInfo()
        -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        return {};
    }

    auto AudioSystemImpl_script::CreateAudioSource(Audio::SAudioInputConfig const& /*sourceConfig*/)
        -> bool
    {
        AZ_Error("AudioSystemImpl_script", false, "Creating an audio source is not supported\n");
        return false;
    }

    void AudioSystemImpl_script::DestroyAudioSource(Audio::TAudioSourceId /*sourceId*/)
    {
    }

    void AudioSystemImpl_script::SetPanningMode(Audio::PanningMode mode)
    {
        m_panningMode = mode;
    }
} // namespace BopAudio
