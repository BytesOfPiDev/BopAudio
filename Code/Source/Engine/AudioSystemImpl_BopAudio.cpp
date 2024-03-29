#include "Engine/AudioSystemImpl_BopAudio.h"

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Engine/MiniAudioEngine.h"
#include "Engine/MiniAudioEngineBus.h"
#include "Engine/MiniAudioEngineRequests.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "MiniAudioIncludes.h"

namespace BopAudio
{
    char const* const AudioSystemImpl_BopAudio::BopAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_BopAudio::AudioSystemImpl_BopAudio(AZStd::string_view assetsPlatformName)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(
            serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);
        m_audioEntityContext = AZStd::make_unique<AzFramework::EntityContext>(serializeContext);

        if (!assetsPlatformName.empty())
        {
            m_assetsPlatform = assetsPlatformName;
        }

        SetPaths();

        Audio::AudioSystemImplementationRequestBus::Handler::BusConnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusConnect();
    }

    AudioSystemImpl_BopAudio::~AudioSystemImpl_BopAudio()
    {
        Audio::AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AudioSystemImpl_BopAudio::SetPaths()
    {
        // "sounds/bopaudio/"
        AZ::IO::Path libraryPath = DefaultBanksPath;

        // "sounds/bopaudio/bopaudio_config.json"
        auto const configFilePath = libraryPath / ConfigFile;

        if (AZ::IO::FileIOBase::GetInstance() &&
            AZ::IO::FileIOBase::GetInstance()->Exists(configFilePath.c_str()))
        {
            ConfigurationSettings configSettings{};
            if (configSettings.Load(configFilePath))
            {
                AZStd::string platformPath{};
                // HACK: Manually setting to linux. I'll implement the platform
                // detection once that works.
                //
                // "sounds/bopaudio/linux"
                AZ::StringFunc::AssetDatabasePath::Join(libraryPath.c_str(), "linux", platformPath);
                AZStd::string initLibraryPath{};
                AZLOG_INFO("InitLibraryPath set: %s.", initLibraryPath.c_str());
                // "sounds/bopaudio/linux/init.soundlib"
                AZ::StringFunc::AssetDatabasePath::Join(
                    platformPath.c_str(), InitBank, initLibraryPath);
                AZLOG_INFO("InitLibraryPath changed: %s.", initLibraryPath.c_str());
                if (AZ::IO::FileIOBase::GetInstance()->Exists(initLibraryPath.c_str()))
                {
                    if (!platformPath.ends_with(AZ_CORRECT_DATABASE_SEPARATOR))
                    {
                        platformPath.push_back(AZ_CORRECT_DATABASE_SEPARATOR);
                    }
                    libraryPath = platformPath;
                }
            }
        }
        else
        {
            AZLOG_ERROR(
                "Failed to find Bop Audio configuration file: \"%s\".",
                configFilePath.c_str()); // NOLINT
        }

        m_soundBankFolder = libraryPath;
        m_localizedSoundBankFolder = libraryPath;

        SetBanksRootPath(m_soundBankFolder);
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemLoseFocus()
    {
        AZLOG_INFO("Bop Audio lost focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemGetFocus()
    {
        AZLOG_INFO("Bop Audio has focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemMuteAll()
    {
        AZLOG_INFO("Bop Audio received MuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemUnmuteAll()
    {
        AZLOG_INFO("Bop Audio received UnmuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemRefresh()
    {
        AZLOG_INFO("Bop Audio received Refresh. Unimplemented.\n");
    }

    void AudioSystemImpl_BopAudio::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_BopAudio::Initialize() -> Audio::EAudioRequestStatus
    {
        if (!AudioEngineInterface::Get()->Initialize())
        {
            AZ_Error("AudioSystemImpl_BopAudio", false, "Failed to initialize MiniAudioEngine!");
            return Audio::EAudioRequestStatus::Failure;
        }

        m_audioEntityContext->InitContext();

        AZLOG_INFO("Bop Audio System Implementation initialized."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        AudioEngineInterface::Get()->Shutdown();

        m_audioEntityContext->DestroyContext();
        m_assetHandlers.clear();

        AZLOG_INFO("Bop Audio shutdown.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::Release() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received Release.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Bop Audio received StopAllSounds.\n");

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::RegisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData, char const* const objectName)
        -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: RegisterAudioObject. [objectName: %s.].", objectName);

        if (!audioObjectData)
        {
            AZLOG_ERROR("BopAudio: RegisterAudioObject failed."); // NOLINT
            return Audio::EAudioRequestStatus::Failure;
        }

        auto* const implAudioObjectData =
            static_cast<SATLAudioObjectData_BopAudio*>(audioObjectData);
        implAudioObjectData->m_name = objectName;
        implAudioObjectData->m_id = AudioEngineInterface::Get()->CreateAudioObject();

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnregisterAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("BopAudio: UnRegisterAudioObject");

        if (!audioObjectData)
        {
            AZLOG_ERROR("BopAudio: UnRegisterAudioObject failed.");
            return Audio::EAudioRequestStatus::Failure;
        }

        auto* const implOldObjectData = static_cast<SATLAudioObjectData_BopAudio*>(audioObjectData);

        AudioEngineInterface::Get()->RemoveAudioObject(implOldObjectData->m_id);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UpdateAudioObject(
        Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const /*triggerData*/) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }
    auto AudioSystemImpl_BopAudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLTriggerImplData const* const triggerData,
        Audio::IATLEventData* const eventData,
        Audio::SATLSourceData const* const pSourceData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ActivateTrigger");

        [[maybe_unused]] auto const* const implTriggerData{
            static_cast<SATLTriggerImplData_BopAudio const*>(triggerData)
        };
        [[maybe_unused]] auto* implAudioObjectData{ static_cast<SATLAudioObjectData_BopAudio*>(
            audioObjectData) };
        [[maybe_unused]] auto* implEventData{ static_cast<SATLEventData_BopAudio*>(eventData) };

        ActivateTriggerRequest activateTriggerRequest{};
        activateTriggerRequest.m_triggerId =
            implTriggerData ? implTriggerData->m_id : INVALID_AUDIO_TRIGGER_IMPL_ID;
        activateTriggerRequest.m_soundName =
            implTriggerData ? implTriggerData->m_soundName : AZ::Name{};
        activateTriggerRequest.m_audioObjectId =
            implAudioObjectData ? implAudioObjectData->m_id : INVALID_AUDIO_OBJECT_ID;

        activateTriggerRequest.m_eventData = implEventData;

        AZ_UNUSED(audioObjectData, triggerData, eventData, pSourceData);
        return AudioEngineInterface::Get()->ActivateTrigger(activateTriggerRequest)
            ? Audio::EAudioRequestStatus::Success
            : Audio::EAudioRequestStatus::Failure;
    }

    auto AudioSystemImpl_BopAudio::StopEvent(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLEventData const* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: StopEvent.\n");
        AZ_UNUSED(audioObjectData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: StopAllEvents.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] Audio::SATLWorldPosition const& worldPosition)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetPosition.\n");
        [[maybe_unused]] auto bopAudioObjectData{ static_cast<SATLAudioObjectData_BopAudio*>(
            audioObjectData) };

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::MultiPositionParams const& multiPositionParams) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLEnvironmentImplData const* const environmentData,
        float const amount) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData,
        float const value) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLSwitchStateImplData const* const switchStateData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData,
        float const obstruction,
        float const occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, Audio::SATLWorldPosition const& newPosition)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData,
        Audio::IATLRtpcImplData const* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::RegisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: RegisterInMemoryFile.\n");

        AudioEngineInterface::Get()->LoadSoundBank(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnregisterInMemoryFile(
        Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: UnregisterInMemoryFile.");
        AZ_UNUSED(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode,
        Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "BopAudio: ParseAudioFileEntry.");

        constexpr auto defaultFileEntry = [](Audio::SATLAudioFileEntryInfo* const entry) -> void
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
        implAudioFile->m_bankId = AZ::Name(soundBankFileName);

        fileEntryInfo->sFileName = soundBankFileName;
        fileEntryInfo->pImplData = implAudioFile;
        fileEntryInfo->nMemoryBlockAlignment = 1;
        fileEntryInfo->bLocalized = false;

        AZ_Assert(
            AZ::Name(fileEntryInfo->sFileName) == implAudioFile->m_bankId,
            "Mismatch Id and FileName! They must compare and be equal!");

        return Audio::EAudioRequestStatus::Success;
    }

    void AudioSystemImpl_BopAudio::DeleteAudioFileEntryData(
        Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioFileEntryData.");
        azdestroy(
            oldAudioFileEntryData, Audio::AudioImplAllocator, SATLAudioFileEntryData_BopAudio);
    }

    auto AudioSystemImpl_BopAudio::GetAudioFileLocation(
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

    auto AudioSystemImpl_BopAudio::NewAudioTriggerImplData(
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

        auto const baId = Audio::AudioStringToID<BA_UniqueId>(triggerName.GetCStr());

        bool const hasValidId{ baId != InvalidBaUniqueId };
        AZ_Error(
            "AudioSystemImpl_BopAudio",
            hasValidId,
            "Failed to create a new audio trigger with ID '%u'.",
            static_cast<AZ::u32>(baId));

        auto* implAudioTriggerData{ azcreate(
            SATLTriggerImplData_BopAudio, (baId), Audio::AudioImplAllocator) };
        implAudioTriggerData->m_id = baId;
        implAudioTriggerData->m_soundName = triggerName; // For now, they're the same. Eventually it
                                                         // will be different.

        return implAudioTriggerData;
    }

    void AudioSystemImpl_BopAudio::DeleteAudioTriggerImplData(
        Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioTriggerImplData.");
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, SATLTriggerImplData_BopAudio);
    }

    auto AudioSystemImpl_BopAudio::NewAudioRtpcImplData(
        const AZ::rapidxml::xml_node<char>* /*audioRtpcNode*/) -> Audio::IATLRtpcImplData*
    {
        AZLOG_ERROR("BopAudio: NewAudioRtpcImplData.");
        return nullptr;
    }

    void AudioSystemImpl_BopAudio::DeleteAudioRtpcImplData(
        Audio::IATLRtpcImplData* const oldRtpcImplData)
    {
        AZLOG_ERROR("BopAudio: DeleteAudioRtpcImplData. Not implemented.");
        azdestroy(oldRtpcImplData, Audio::AudioImplAllocator, SATLRtpcImplData_BopAudio);
    }

    auto AudioSystemImpl_BopAudio::NewAudioSwitchStateImplData(
        const AZ::rapidxml::xml_node<char>* audioSwitchStateNode) -> Audio::IATLSwitchStateImplData*
    {
        AZLOG_ERROR("BopAudio: NewAudioSwitchStateImplData. Not implemented.");
        AZ_UNUSED(audioSwitchStateNode);
        return nullptr;
    }

    void AudioSystemImpl_BopAudio::DeleteAudioSwitchStateImplData(
        Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioSwitchStateImplData.");
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_BopAudio::NewAudioEnvironmentImplData(
        const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) -> Audio::IATLEnvironmentImplData*
    {
        AZLOG(
            ASI_BopAudio,
            "BopAudio: NewAudioEnvironmentImplData. [audioEnvironmentNode: "
            "%s].",
            audioEnvironmentNode->name());
        return nullptr;
    }

    void AudioSystemImpl_BopAudio::DeleteAudioEnvironmentImplData(
        Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator);
    }

    auto AudioSystemImpl_BopAudio::NewGlobalAudioObjectData(Audio::TAudioObjectID const objectId)
        -> Audio::IATLAudioObjectData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewGlobalAudioObjectData. [objectId: %llu].", objectId);
        return {};
    }

    auto AudioSystemImpl_BopAudio::NewAudioObjectData(Audio::TAudioObjectID const objectId)
        -> Audio::IATLAudioObjectData*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioObjectData. [objectId: %llu].", objectId);

        return azcreate(SATLAudioObjectData_BopAudio, (objectId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_BopAudio::DeleteAudioObjectData(
        Audio::IATLAudioObjectData* const oldObjectData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioObjectData.");

        azdestroy(oldObjectData, Audio::AudioImplAllocator, SATLAudioObjectData_BopAudio);
    }

    auto AudioSystemImpl_BopAudio::NewDefaultAudioListenerObjectData(
        Audio::TATLIDType const objectId) -> SATLListenerData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewDefaultAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_BopAudio::NewAudioListenerObjectData(Audio::TATLIDType const objectId)
        -> SATLListenerData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioListenerObjectData.");
        AZ_UNUSED(objectId);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioListenerObjectData(
        Audio::IATLListenerData* const oldListenerData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioListenerObjectData.");
        AZ_UNUSED(oldListenerData);
    }

    auto AudioSystemImpl_BopAudio::NewAudioEventData(Audio::TAudioEventID const eventId)
        -> SATLEventData_BopAudio*
    {
        AZLOG(ASI_BopAudio, "BopAudio: NewAudioEventData");
        return azcreate(SATLEventData_BopAudio, (eventId), Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_BopAudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DeleteAudioEventData.");
        azdestroy(oldEventData, Audio::AudioImplAllocator);
    }

    void AudioSystemImpl_BopAudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZLOG(ASI_BopAudio, "BopAudio: ResetAudioEventData.");

        if (!eventData)
        {
            AZ_Error(
                "AudioSystemImpl_BopAudio", false, "Received nullptr. Valid address required.");
            return;
        }

        auto* const bopEventData{ static_cast<SATLEventData_BopAudio*>(eventData) };

        eventData->m_owner = nullptr;
        eventData->m_triggerId = INVALID_AUDIO_CONTROL_ID;
        bopEventData->m_audioEventState = Audio::EAudioEventState::eAES_NONE;
        bopEventData->m_sourceId = INVALID_AUDIO_CONTROL_ID;
        bopEventData->m_baId = InvalidBaUniqueId;
    }

    auto AudioSystemImpl_BopAudio::GetImplSubPath() const -> char const* const
    {
        return BopAudioImplSubPath;
    }

    void AudioSystemImpl_BopAudio::SetLanguage(char const* const language)
    {
        m_language = language;
    }

    // Functions below are only used when WWISE_RELEASE is not defined
    auto AudioSystemImpl_BopAudio::GetImplementationNameString() const -> char const* const
    {
        return "BopAudio-Dev";
    }

    void AudioSystemImpl_BopAudio::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_BopAudio::GetMemoryPoolInfo()
        -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        AZLOG(ASI_BopAudio, "BopAudio: GetMemoryInfo.");
        return {};
    }

    auto AudioSystemImpl_BopAudio::CreateAudioSource(
        Audio::SAudioInputConfig const& /*sourceConfig*/) -> bool
    {
        AZLOG(ASI_BopAudio, "BopAudio: CreateAudioSource.");
        return true;
    }

    void AudioSystemImpl_BopAudio::DestroyAudioSource(Audio::TAudioSourceId sourceId)
    {
        AZLOG(ASI_BopAudio, "BopAudio: DestroyAudioSource.");
        AZ_UNUSED(sourceId);
    }

    void AudioSystemImpl_BopAudio::SetPanningMode(Audio::PanningMode mode)
    {
        m_panningMode = mode;
    }
} // namespace BopAudio
