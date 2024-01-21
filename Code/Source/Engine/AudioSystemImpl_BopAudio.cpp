#include "AzCore/IO/FileIO.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Engine/ATLEntities_BopAudio.h"

#include "AzCore/Console/ILogger.h"
#include "AzCore/base.h"
#include "Engine/ConfigurationSettings.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/AudioAsset.h"
#include "Engine/AudioSystemImpl_BopAudio.h"

namespace BopAudio
{
    char const* const AudioSystemImpl_BopAudio::BopAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_BopAudio::AudioSystemImpl_BopAudio(AZStd::string_view assetsPlatformName)
    {
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
        AZStd::string libraryPath = DefaultLibrariesPath;

        // "sounds/bopaudio/bopaudio_config.json"
        auto const configFilename = libraryPath + ConfigFile; //! TODO: Compile-time evaluate

        if (AZ::IO::FileIOBase::GetInstance() && AZ::IO::FileIOBase::GetInstance()->Exists(configFilename.c_str()))
        {
            ConfigurationSettings configSettings{};
            if (configSettings.Load(configFilename))
            {
                AZStd::string platformPath{};
                // HACK: Manually setting to linux. I'll implement the platform detection once that works.
                //
                // "sounds/bopaudio/linux"
                AZ::StringFunc::AssetDatabasePath::Join(libraryPath.c_str(), "linux", platformPath);
                AZStd::string initLibraryPath{};
                // "sounds/bopaudio/linux/init.balib"
                AZ::StringFunc::AssetDatabasePath::Join(platformPath.c_str(), InitLibrary, initLibraryPath);
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
            AZLOG_ERROR("Failed to find Steam Audio configuration file: \"%s\".", configFilename.c_str()); // NOLINT
        }

        m_soundLibraryFolder = libraryPath;
        m_localizedSoundLibraryFolder = libraryPath;

        SetLibrariesRootPath(m_soundLibraryFolder);
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemLoseFocus()
    {
        AZLOG_INFO("Steam Audio lost focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemGetFocus()
    {
        AZLOG_INFO("Steam Audio has focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemMuteAll()
    {
        AZLOG_INFO("Steam Audio received MuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemUnmuteAll()
    {
        AZLOG_INFO("Steam Audio received UnmuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_BopAudio::OnAudioSystemRefresh()
    {
        AZLOG_INFO("Steam Audio received Refresh. Unimplemented.\n");
    }

    void AudioSystemImpl_BopAudio::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_BopAudio::Initialize() -> Audio::EAudioRequestStatus
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);

        AZLOG_INFO("Bop Audio System Implementation initialized."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        m_assetHandlers.clear();

        AZLOG_INFO("Steam Audio shutdown."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::Release() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Steam Audio received Release.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Steam Audio received StopAllSounds. Unimplemented.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::RegisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData, const char* const objectName)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioObjectData, objectName);

        if (!audioObjectData)
        {
            AZLOG_ERROR("RegisterAudioObject failed."); // NOLINT
            return Audio::EAudioRequestStatus::Failure;
        }

        // auto miniaudio_audioObjectData = static_cast<SATLAudioObjectData_miniaudio*>(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnregisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnregisterAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UpdateAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UpdateAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const /*triggerData*/)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }
    auto AudioSystemImpl_BopAudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return {};
    }

    Audio::EAudioRequestStatus AudioSystemImpl_BopAudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData)
    {
        AZLOG(ASI_miniaudio, "PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData,
        const Audio::SATLSourceData* const pSourceData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ActivateTrigger.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData, pSourceData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::StopEvent(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEventData* const eventData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "StopEvent.\n");
        AZ_UNUSED(audioObjectData, eventData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "StopAllEvents.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::SATLWorldPosition& worldPosition) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetPosition.\n");
        AZ_UNUSED(audioObjectData, worldPosition);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::MultiPositionParams& multiPositionParams)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEnvironmentImplData* const environmentData, const float amount)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData, const float value)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLSwitchStateImplData* const switchStateData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return {};
    }

    auto AudioSystemImpl_BopAudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, const Audio::SATLWorldPosition& newPosition) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetListenerPosition.\n");
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::RegisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "RegisterInMemoryFile.\n");
        AZ_UNUSED(audioFileEntry);
        return {};
    }

    auto AudioSystemImpl_BopAudio::UnregisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntry);
        return {};
    }

    auto AudioSystemImpl_BopAudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode, Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntryNode, fileEntryInfo);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioFileEntryData(Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZ_UNUSED(oldAudioFileEntryData);
    }

    auto AudioSystemImpl_BopAudio::GetAudioFileLocation(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> const char* const
    {
        AZ_UNUSED(fileEntryInfo);
        return {};
    }

    auto AudioSystemImpl_BopAudio::NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode)
        -> Audio::IATLTriggerImplData*
    {
        AZ_UNUSED(audioTriggerNode);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioTriggerImplData(Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZ_UNUSED(oldTriggerImplData);
    }

    auto AudioSystemImpl_BopAudio::NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode) -> Audio::IATLRtpcImplData*
    {
        AZ_UNUSED(audioRtpcNode);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioRtpcImplData(Audio::IATLRtpcImplData* const oldRtpcImplData)
    {
        AZ_UNUSED(oldRtpcImplData);
    }

    auto AudioSystemImpl_BopAudio::NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
        -> Audio::IATLSwitchStateImplData*
    {
        AZ_UNUSED(audioSwitchStateNode);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioSwitchStateImplData(Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZ_UNUSED(oldSwitchStateImplData);
    }

    auto AudioSystemImpl_BopAudio::NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
        -> Audio::IATLEnvironmentImplData*
    {
        AZ_UNUSED(audioEnvironmentNode);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioEnvironmentImplData(Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        AZ_UNUSED(oldEnvironmentImplData);
    }

    auto AudioSystemImpl_BopAudio::NewGlobalAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_BopAudio::NewAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData*
    {
        return aznew SATLAudioObjectData_BopAudio(objectId, false);
    }

    void AudioSystemImpl_BopAudio::DeleteAudioObjectData(Audio::IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData);
    }

    auto AudioSystemImpl_BopAudio::NewDefaultAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_BopAudio*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_BopAudio::NewAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_BopAudio*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioListenerObjectData(Audio::IATLListenerData* const oldListenerData)
    {
        AZ_UNUSED(oldListenerData);
    }

    auto AudioSystemImpl_BopAudio::NewAudioEventData(const Audio::TAudioEventID eventId) -> SATLEventData_BopAudio*
    {
        AZ_UNUSED(eventId);
        return {};
    }

    void AudioSystemImpl_BopAudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZ_UNUSED(oldEventData);
    }

    void AudioSystemImpl_BopAudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZ_UNUSED(eventData);
    }

    auto AudioSystemImpl_BopAudio::GetImplSubPath() const -> const char* const
    {
        return BopAudioImplSubPath;
    }

    void AudioSystemImpl_BopAudio::SetLanguage(const char* const language)
    {
        AZ_UNUSED(language);
    }

    // Functions below are only used when WWISE_RELEASE is not defined
    auto AudioSystemImpl_BopAudio::GetImplementationNameString() const -> char const* const
    {
        return "MiniAudio-Dev";
    }

    void AudioSystemImpl_BopAudio::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_BopAudio::GetMemoryPoolInfo() -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        return {};
    }

    auto AudioSystemImpl_BopAudio::CreateAudioSource(const Audio::SAudioInputConfig& sourceConfig) -> bool
    {
        AZ_UNUSED(sourceConfig);
        return {};
    }

    void AudioSystemImpl_BopAudio::DestroyAudioSource(Audio::TAudioSourceId sourceId)
    {
        AZ_UNUSED(sourceId);
    }

    void AudioSystemImpl_BopAudio::SetPanningMode(Audio::PanningMode mode)
    {
        AZ_UNUSED(mode);
    }
} // namespace BopAudio
