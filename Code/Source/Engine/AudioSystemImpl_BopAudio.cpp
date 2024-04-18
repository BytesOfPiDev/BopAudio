#include "Engine/AudioSystemImpl_BopAudio.h"

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/BopAudioBus.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"
#include "MiniAudio/SoundAsset.h"

namespace BopAudio
{
    char const* const AudioSystemImpl_miniaudio::BopAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_miniaudio::AudioSystemImpl_miniaudio(AZStd::string_view assetsPlatformName)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);

        if (!assetsPlatformName.empty())
        {
            m_assetsPlatform = assetsPlatformName;
        }

        SetPaths();

        Audio::AudioSystemImplementationRequestBus::Handler::BusConnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusConnect();

        AsiInterface::Register(this);
    }

    AudioSystemImpl_miniaudio::~AudioSystemImpl_miniaudio()
    {
        AsiInterface::Unregister(this);

        Audio::AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AudioSystemImpl_miniaudio::SetPaths()
    {
        // "sounds/bopaudio/"

        // "sounds/bopaudio/bopaudio_config.json"
        auto const configFilePath = AZ::IO::Path{ ProjectAlias } / ConfigFile;

        if (AZ::IO::FileIOBase::GetInstance() &&
            AZ::IO::FileIOBase::GetInstance()->Exists(configFilePath.c_str()))
        {
            ConfigurationSettings configSettings{};
            if (configSettings.Load(configFilePath))
            {
                m_localizedSoundBankFolder = {
                    m_soundBankFolder / LanguageNamespace::ToString(configSettings.GetLanguage())
                };
            }
        }
        else
        {
            AZLOG_ERROR(
                "Failed to find Bop Audio configuration file: \"%s\".", configFilePath.c_str());
        };

        if (AZ::IO::FileIOBase::GetInstance() == nullptr)
        {
            AZLOG_ERROR(
                "ASI is unable to set the soundbank folder because due to no FileIO instance.");

            return;
        }

        m_soundBankFolder = BanksAlias;
        SetBanksRootPath(m_soundBankFolder);
    }

    void AudioSystemImpl_miniaudio::OnAudioSystemLoseFocus()
    {
        AZLOG_INFO("Bop Audio lost focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_miniaudio::OnAudioSystemGetFocus()
    {
        AZLOG_INFO("Bop Audio has focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_miniaudio::OnAudioSystemMuteAll()
    {
        AZLOG_INFO("Bop Audio received MuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_miniaudio::OnAudioSystemUnmuteAll()
    {
        AZLOG_INFO("Bop Audio received UnmuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_miniaudio::OnAudioSystemRefresh()
    {
        AZLOG_INFO("Bop Audio received Refresh. Unimplemented.\n");
    }

    void AudioSystemImpl_miniaudio::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_miniaudio::Initialize() -> Audio::EAudioRequestStatus
    {
        if (!SoundEngine::Get())
        {
            AZ_Error("ASI", false, "No MiniAudio AudioEngineInterface found!");
            return Audio::EAudioRequestStatus::Failure;
        }

        auto const initializeOutcome{ SoundEngine::Get()->Initialize() };
        if (!initializeOutcome.IsSuccess())
        {
            AZ_Error(
                "AudioSystemImpl_MiniAudio",
                false,
                "Failed to initialize MiniAudioEngine! Reason: [%s]",
                initializeOutcome.GetError().c_str());

            return Audio::EAudioRequestStatus::Failure;
        }

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        return SoundEngine::Get()->Shutdown() ? Audio::EAudioRequestStatus::Success
                                              : Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_miniaudio::Release() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received Release.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received StopAllSounds.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::RegisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData, char const* const objectName)
        -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: RegisterAudioObject. [objectName: %s.].", objectName);

        if (!audioObjectData)
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio",
                false,
                "Failed to register audio object. Received nullptr.");

            return Audio::EAudioRequestStatus::Failure;
        }

        auto* const implAudioObjectData =
            static_cast<SATLAudioObjectData_BopAudio*>(audioObjectData);

        implAudioObjectData->ChangeName(objectName);
        implAudioObjectData->SetImplAudioObjectId(SoundEngine::Get()->CreateAudioObject());

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::UnregisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: UnRegisterAudioObject");

        if (!audioObjectData)
        {
            AZLOG_ERROR("BopAudio: UnRegisterAudioObject failed.");
            return Audio::EAudioRequestStatus::Failure;
        }

        auto* const implOldObjectData = static_cast<SATLAudioObjectData_BopAudio*>(audioObjectData);

        SoundEngine::Get()->RemoveAudioObject(static_cast<AZ::u32>(implOldObjectData->GetAtlAudioObjectId()));
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::ResetAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::UpdateAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const /*triggerData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }
    auto AudioSystemImpl_miniaudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData,
        Audio::SATLSourceData const* const /*pSourceData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(LOG_ASI, "Received ActivateTrigger");

        [[maybe_unused]] auto* const implTriggerData{
            static_cast<SATLTriggerImplData_BopAudio const*>(triggerData)
        };
        auto* const implAudioObjectData{ static_cast<SATLAudioObjectData_BopAudio*>(
            audioObjectData) };
        auto* const implEventData{ static_cast<SATLEventData_BopAudio*>(eventData) };

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

        if (!implAudioObjectData->GetImplAudioObjectId().IsValid())
        {
            AZ_Error("ASI", false, "Invalid impl object id.");
            return Audio::EAudioRequestStatus::FailureInvalidObjectId;
        }

        if (implAudioObjectData->GetAtlAudioObjectId() == INVALID_AUDIO_OBJECT_ID)
        {
            AZ_Error("ASI", false, "Invalid control id.");
            return Audio::EAudioRequestStatus::FailureInvalidControlId;
        }

        implEventData->SetImplEventId(implTriggerData->GetEventId());

        StartEventData activateTriggerRequest{};
        activateTriggerRequest.m_owner = eventData->m_owner;
        activateTriggerRequest.m_audioControlId = eventData->m_triggerId;
        activateTriggerRequest.m_audioObjectId = implAudioObjectData->GetImplAudioObjectId();
        activateTriggerRequest.m_audioEventId = implTriggerData->GetEventId();

        auto const activateOutcome{ SoundEngine::Get()->StartEvent(activateTriggerRequest) };

        AZ_Error(
            "ASI",
            activateOutcome.IsSuccess(),
            "Failed to activate event. [EventId: %zu]': '[Error: %s]'",
            implTriggerData->GetEventId(),
            activateOutcome.GetError().c_str());

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::StopEvent(
        Audio::IATLAudioObjectData* const /*audioObjectData*/,
        Audio::IATLEventData const* const eventData) -> Audio::EAudioRequestStatus
    {
        auto const* const implEventData{ static_cast<SATLEventData_BopAudio const*>(eventData) };

        auto const isStopped{ SoundEngine::Get()->StopEvent(implEventData->GetImplEventId()) };

        return isStopped ? Audio::EAudioRequestStatus::Success
                         : Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_miniaudio::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData)
        -> Audio::EAudioRequestStatus
    {
        AZ_Error("ASI", false, "StopAllEvents is not yet implemented.");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_miniaudio::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] Audio::SATLWorldPosition const& worldPosition)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetPosition.\n");
        [[maybe_unused]] auto bopAudioObjectData{ static_cast<SATLAudioObjectData_BopAudio*>(
            audioObjectData) };

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::MultiPositionParams const& multiPositionParams) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLEnvironmentImplData const* const environmentData,
        float const amount) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData,
        float const value) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLSwitchStateImplData const* const switchStateData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData,
        float const obstruction,
        float const occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, Audio::SATLWorldPosition const& newPosition)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::RegisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: RegisterInMemoryFile.\n");

        if (!audioFileEntry)
        {
            AZ_Error("AudioSystemImpl_MiniAudio", false, "Given audio file entry is nullptr");
            return Audio::EAudioRequestStatus::Failure;
        }

        auto const loadBankOutcome = [&audioFileEntry]()
        {
            auto fileBuffer{ AZStd::span<char const>(
                static_cast<char*>(audioFileEntry->pFileData), audioFileEntry->nSize) };

            return SoundEngine::Get()->RegisterFile({ fileBuffer, audioFileEntry->sFileName });
        }();

        if (!loadBankOutcome.IsSuccess())
        {
            AZ_Error(
                "AudioSystemImpl_MiniAudio",
                false,
                "Failed to register in-memory file. Reason: [%s]",
                loadBankOutcome.GetError().c_str());

            return Audio::EAudioRequestStatus::Failure;
        }

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::UnregisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnregisterInMemoryFile.");
        AZ_UNUSED(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_miniaudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode,
        Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ParseAudioFileEntry.");

        static constexpr auto defaultFileEntry =
            [](Audio::SATLAudioFileEntryInfo* const entry) -> void
        {
            entry->sFileName = nullptr;
            entry->pFileData = nullptr;
            entry->pImplData = nullptr;
            entry->bLocalized = false;
            entry->nMemoryBlockAlignment = 0;
            entry->nSize = 0;
        };

        if (!audioFileEntryNode ||
            !AZ::StringFunc::Equal(audioFileEntryNode->name(), XmlTags::BopFileTag))
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio",
                false,
                "Wrong node name. Expected %s, but got %s.",
                XmlTags::BopFileTag,
                audioFileEntryNode->name());

            defaultFileEntry(fileEntryInfo);

            return Audio::EAudioRequestStatus::FailureInvalidRequest;
        }

        auto const* const bopAudioFileNameAttrib =
            audioFileEntryNode->first_attribute(XmlTags::NameAttribute);

        if (!bopAudioFileNameAttrib)
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio",
                false,
                "Missing attribute. Expected %s.",
                XmlTags::NameAttribute);
            defaultFileEntry(fileEntryInfo);
            return Audio::EAudioRequestStatus::FailureInvalidRequest;
        }

        auto* const soundBankFileName{ bopAudioFileNameAttrib->value() };
        auto* const implAudioFile{ azcreate(
            SATLAudioFileEntryData_BopAudio, (), Audio::AudioImplAllocator) };
        implAudioFile->m_bankId = ResourceRef{ soundBankFileName };

        fileEntryInfo->sFileName = soundBankFileName;
        fileEntryInfo->pImplData = implAudioFile;
        fileEntryInfo->nMemoryBlockAlignment = 1;
        fileEntryInfo->bLocalized = false;

        return Audio::EAudioRequestStatus::Success;
    }

    void AudioSystemImpl_miniaudio::DeleteAudioFileEntryData(
        Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioFileEntryData.");
        azdestroy(
            oldAudioFileEntryData, Audio::AudioImplAllocator, SATLAudioFileEntryData_BopAudio);
    }

    auto AudioSystemImpl_miniaudio::GetAudioFileLocation(
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

    auto AudioSystemImpl_miniaudio::NewAudioTriggerImplData(
        const AZ::rapidxml::xml_node<char>* audioTriggerNode) -> Audio::IATLTriggerImplData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioTriggerImplData.");

        if (!audioTriggerNode ||
            !AZ::StringFunc::Equal(audioTriggerNode->name(), XmlTags::TriggerTag))
        {
            return nullptr;
        }

        AZ::Name const triggerName = [&audioTriggerNode]() -> decltype(triggerName)
        {
            auto const triggerNameAttrib{ audioTriggerNode->first_attribute(
                XmlTags::NameAttribute) };
            return decltype(triggerName){ triggerNameAttrib ? triggerNameAttrib->value()
                                                            : nullptr };
        }();

        auto* implAudioTriggerData{ azcreate(
            SATLTriggerImplData_BopAudio, (), Audio::AudioImplAllocator) };

        implAudioTriggerData->SetImplEventId(
            Audio::AudioStringToID<Audio::TAudioEventID>(triggerName.GetCStr()));

        return implAudioTriggerData;
    }

    void AudioSystemImpl_miniaudio::DeleteAudioTriggerImplData(
        Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZLOG(LOG_ASI, "ASI: DeleteAudioTriggerImplData called");
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, SATLTriggerImplData_BopAudio);
    }

    auto AudioSystemImpl_miniaudio::NewAudioRtpcImplData(
        const AZ::rapidxml::xml_node<char>* /*audioRtpcNode*/) -> Audio::IATLRtpcImplData*
    {
        AZ_Error("ASI", false, "NewAudioRtpcImplData is not yet implemented");
        return nullptr;
    }

    void AudioSystemImpl_miniaudio::DeleteAudioRtpcImplData(
        Audio::IATLRtpcImplData* const /*oldRtpcImplData*/)
    {
        AZ_Error("ASI", false, "DeleteAudioRtpcImplData is not yet implemented");
    }

    auto AudioSystemImpl_miniaudio::NewAudioSwitchStateImplData(
        const AZ::rapidxml::xml_node<char>* /*audioSwitchStateNode*/)
        -> Audio::IATLSwitchStateImplData*
    {
        AZ_Error(
            "AudioSystemImpl_MiniAudio",
            false,
            "NewAudioSwitchStateImplData is not yet implemented.");
        return nullptr;
    }

    void AudioSystemImpl_miniaudio::DeleteAudioSwitchStateImplData(
        Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZLOG_INFO("ASI: DeleteAudioSwitchStateImplData.");
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_miniaudio::NewAudioEnvironmentImplData(
        const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) -> Audio::IATLEnvironmentImplData*
    {
        AZLOG(
            ASI_BopAudio,
            "BopAudio: NewAudioEnvironmentImplData. [audioEnvironmentNode: "
            "%s].",
            audioEnvironmentNode->name());
        return nullptr;
    }

    void AudioSystemImpl_miniaudio::DeleteAudioEnvironmentImplData(
        Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_miniaudio::NewGlobalAudioObjectData(
        Audio::TAudioObjectID const atlObjectId) -> Audio::IATLAudioObjectData*
    {
        AZLOG_INFO("ASI: NewGlobalAudioObjectData. [objectId: %llu].", atlObjectId);
        return azcreate(
            SATLAudioObjectData_BopAudio,
            (atlObjectId, GlobalAudioObjectId),
            Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_miniaudio::NewAudioObjectData(Audio::TAudioObjectID const atlObjectId)
        -> Audio::IATLAudioObjectData*
    {
        return azcreate(
            SATLAudioObjectData_BopAudio,
            (atlObjectId, static_cast<AZ::u32>(atlObjectId)),
            Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_miniaudio::DeleteAudioObjectData(
        Audio::IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData, Audio::AudioImplAllocator, SATLAudioObjectData_BopAudio);
    }

    auto AudioSystemImpl_miniaudio::NewDefaultAudioListenerObjectData(
        Audio::TATLIDType const objectId) -> SATLListenerData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewDefaultAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_miniaudio::NewAudioListenerObjectData(Audio::TATLIDType const objectId)
        -> SATLListenerData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    void AudioSystemImpl_miniaudio::DeleteAudioListenerObjectData(
        Audio::IATLListenerData* const oldListenerData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioListenerObjectData.");
        AZ_UNUSED(oldListenerData);
    }

    auto AudioSystemImpl_miniaudio::NewAudioEventData(Audio::TAudioEventID const eventId)
        -> SATLEventData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioEventData");
        return azcreate(SATLEventData_BopAudio, (eventId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_miniaudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioEventData.");
        azdestroy(oldEventData, Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_miniaudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioEventData.");

        if (!eventData)
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio", false, "Received nullptr. Valid address required.");
            return;
        }

        auto* const bopEventData{ static_cast<SATLEventData_BopAudio*>(eventData) };

        // eventData->m_owner = nullptr;
        // eventData->m_triggerId = INVALID_AUDIO_CONTROL_ID;
        // bopEventData->SetEventState(Audio::EAudioEventState::eAES_NONE);
        bopEventData->SetSourceId(INVALID_AUDIO_SOURCE_ID);
    }

    auto AudioSystemImpl_miniaudio::GetImplSubPath() const -> char const* const
    {
        return BopAudioImplSubPath;
    }

    void AudioSystemImpl_miniaudio::SetLanguage(char const* const language)
    {
        m_language = language;
    }

    // Functions below are only used when RELEASE is not defined
    auto AudioSystemImpl_miniaudio::GetImplementationNameString() const -> char const* const
    {
        return "BopAudio-Dev";
    }

    void AudioSystemImpl_miniaudio::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_miniaudio::GetMemoryPoolInfo()
        -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        return {};
    }

    auto AudioSystemImpl_miniaudio::CreateAudioSource(Audio::SAudioInputConfig const& sourceConfig)
        -> bool
    {
        AZ::Data::AssetId const assetId = [&sourceConfig]() -> decltype(assetId)
        {
            auto foundAssetId{ decltype(assetId){} };
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                foundAssetId,
                &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
                sourceConfig.m_sourceFilename.c_str(),
                AZ::AzTypeInfo<MiniAudio::SoundAsset>::Uuid(),
                false);

            return foundAssetId;
        }();

        auto soundAsset{ AZ::Data::AssetManager::Instance().GetAsset<MiniAudio::SoundAsset>(
            assetId, AZ::Data::AssetLoadBehavior::PreLoad) };

        if (soundAsset.QueueLoad())
        {
            AZLOG_ERROR(
                "Failed to queue loading for audio input config with source filename '%s'",
                sourceConfig.m_sourceFilename.c_str());

            return false;
        }

        if (soundAsset.IsError())
        {
            AZLOG_ERROR(
                "An error occurred while loading audio input config with source filename '%s'",
                sourceConfig.m_sourceFilename.c_str());

            return false;
        }

        soundAsset.BlockUntilLoadComplete();

        m_sourceAssetMap[sourceConfig.m_sourceId] = soundAsset;

        auto const& assetBuffer = soundAsset->m_data;

        auto* const engine = SoundEngine::Get()->GetSoundEngine();

        if (!engine)
        {
            AZLOG_ERROR("ASI failed to obtain ma_engine while creating an audio source");
            return false;
        }

        ma_result const result = ma_resource_manager_register_encoded_data(
            ma_engine_get_resource_manager(engine),
            sourceConfig.m_sourceFilename.c_str(),
            assetBuffer.data(),
            assetBuffer.size());

        bool const isSuccess(result == MA_SUCCESS);

        AZLOG_ERROR("ASI failed to register sound with miniaudio resource manager");

        return isSuccess;
    }

    void AudioSystemImpl_miniaudio::DestroyAudioSource(Audio::TAudioSourceId sourceId)
    {
        m_sourceAssetMap.erase(sourceId);
    }

    void AudioSystemImpl_miniaudio::SetPanningMode(Audio::PanningMode mode)
    {
        m_panningMode = mode;
    }
} // namespace BopAudio
