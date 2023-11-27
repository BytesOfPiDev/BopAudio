#include "AzCore/IO/FileIO.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Engine/ATLEntities_SteamAudio.h"

#include "AzCore/Console/ILogger.h"
#include "AzCore/RTTI/RTTIMacros.h"
#include "AzCore/base.h"
#include "Engine/ConfigurationSettings.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"

#include "Engine/AudioSystemImpl_SteamAudio.h"
#include "SteamAudio/AudioAsset.h"

namespace SteamAudio
{
    char const* const AudioSystemImpl_SteamAudio::SteamAudioImplSubPath = "steamaudio/";

    AudioSystemImpl_SteamAudio::AudioSystemImpl_SteamAudio(AZStd::string_view assetsPlatformName)
    {
        if (!assetsPlatformName.empty())
        {
            m_assetsPlatform = assetsPlatformName;
        }

        SetPaths();

        Audio::AudioSystemImplementationRequestBus::Handler::BusConnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusConnect();
    }

    AudioSystemImpl_SteamAudio::~AudioSystemImpl_SteamAudio()
    {
        Audio::AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        Audio::AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AudioSystemImpl_SteamAudio::SetPaths()
    {
        // "sounds/steamaudio/"
        AZStd::string libraryPath = DefaultLibrariesPath;

        // "sounds/steamaudio/steamaudio_config.json"
        AZStd::string configFile = libraryPath + ConfigFile;

        if (AZ::IO::FileIOBase::GetInstance() && AZ::IO::FileIOBase::GetInstance()->Exists(configFile.c_str()))
        {
            ConfigurationSettings configSettings{};
            if (configSettings.Load(configFile))
            {
                AZStd::string platformPath{};
                // HACK: Manually setting to linux. I'll implement the platform detection once that works.
                //
                // "sounds/steamaudio/linux"
                AZ::StringFunc::AssetDatabasePath::Join(libraryPath.c_str(), "linux", platformPath);
                AZStd::string initLibraryPath{};
                // "sounds/steamaudio/linux/init.salib"
                AZ::StringFunc::AssetDatabasePath::Join(platformPath.c_str(), InitLibrary, initLibraryPath);
                if (AZ::IO::FileIOBase::GetInstance()->Exists(initLibraryPath.c_str()))
                {
                    if (!platformPath.ends_with(AZ_CORRECT_DATABASE_SEPARATOR))
                    {
                        platformPath.push_back(AZ_CORRECT_DATABASE_SEPARATOR);
                    }
                    libraryPath = AZStd::move(platformPath);
                }
            }
        }
        else
        {
            AZLOG_ERROR("Failed to find Steam Audio configuration file: \"%s\".", configFile.c_str()); // NOLINT
        }

        m_soundLibraryFolder = libraryPath;
        m_localizedSoundLibraryFolder = libraryPath;

        SetLibrariesRootPath(m_soundLibraryFolder);
    }

    void AudioSystemImpl_SteamAudio::OnAudioSystemLoseFocus()
    {
        AZLOG_INFO("Steam Audio lost focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_SteamAudio::OnAudioSystemGetFocus()
    {
        AZLOG_INFO("Steam Audio has focus. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_SteamAudio::OnAudioSystemMuteAll()
    {
        AZLOG_INFO("Steam Audio received MuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_SteamAudio::OnAudioSystemUnmuteAll()
    {
        AZLOG_INFO("Steam Audio received UnmuteAll. Unimplemented."); // NOLINT
    }

    void AudioSystemImpl_SteamAudio::OnAudioSystemRefresh()
    {
        AZLOG_INFO("Steam Audio received Refresh. Unimplemented.\n");
    }

    void AudioSystemImpl_SteamAudio::Update(float const updateIntervalMS)
    {
        AZ_UNUSED(updateIntervalMS);
    }

    auto AudioSystemImpl_SteamAudio::Initialize() -> Audio::EAudioRequestStatus
    {
        m_contextSettings.version = STEAMAUDIO_VERSION;

        iplContextCreate(&m_contextSettings, &m_context);

        if (!m_context)
        {
            AZLOG_ERROR("Failed to create Steam Audio Context!"); // NOLINT
            return Audio::EAudioRequestStatus::Failure;
        }

        m_hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

        static constexpr auto DefaultSamplingRate = 44100;
        static constexpr auto DefaultFrameSize = 1024;

        m_audioSettings.samplingRate = DefaultSamplingRate;
        m_audioSettings.frameSize = DefaultFrameSize;

        iplHRTFCreate(m_context, &m_audioSettings, &m_hrtfSettings, &m_hrtf);

        if (!m_hrtf)
        {
            AZLOG_ERROR("Failed to create Steam Audio HRTF!"); // NOLINT
            return Audio::EAudioRequestStatus::Failure;
        }

        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);

        AZLOG_INFO("Steam Audio System Implementation initialized."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        m_assetHandlers.clear();
        iplHRTFRelease(&m_hrtf);
        iplContextRelease(&m_context);

        AZLOG_INFO("Steam Audio shutdown."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::Release() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Steam Audio received Release.");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::StopAllSounds() -> Audio::EAudioRequestStatus
    {
        AZLOG_INFO("Steam Audio received StopAllSounds. Unimplemented.\n");
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::RegisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData, const char* const objectName)
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

    auto AudioSystemImpl_SteamAudio::UnregisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnregisterAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::UpdateAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UpdateAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }
    auto AudioSystemImpl_SteamAudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return {};
    }

    Audio::EAudioRequestStatus AudioSystemImpl_SteamAudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData)
    {
        AZLOG(ASI_miniaudio, "PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData,
        const Audio::SATLSourceData* const pSourceData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ActivateTrigger.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData, pSourceData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::StopEvent(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "StopEvent.\n");
        AZ_UNUSED(audioObjectData, eventData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "StopAllEvents.\n");
        AZ_UNUSED(audioObjectData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::SATLWorldPosition& worldPosition) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetPosition.\n");
        AZ_UNUSED(audioObjectData, worldPosition);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::MultiPositionParams& multiPositionParams)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEnvironmentImplData* const environmentData, const float amount)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData, const float value)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLSwitchStateImplData* const switchStateData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, const Audio::SATLWorldPosition& newPosition) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "SetListenerPosition.\n");
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_SteamAudio::RegisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_miniaudio, "RegisterInMemoryFile.\n");
        AZ_UNUSED(audioFileEntry);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::UnregisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntry);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode, Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntryNode, fileEntryInfo);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioFileEntryData(Audio::IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        AZ_UNUSED(oldAudioFileEntryData);
    }

    auto AudioSystemImpl_SteamAudio::GetAudioFileLocation(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> const char* const
    {
        AZ_UNUSED(fileEntryInfo);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode)
        -> Audio::IATLTriggerImplData*
    {
        AZ_UNUSED(audioTriggerNode);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioTriggerImplData(Audio::IATLTriggerImplData* const oldTriggerImplData)
    {
        AZ_UNUSED(oldTriggerImplData);
    }

    auto AudioSystemImpl_SteamAudio::NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode) -> Audio::IATLRtpcImplData*
    {
        AZ_UNUSED(audioRtpcNode);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioRtpcImplData(Audio::IATLRtpcImplData* const oldRtpcImplData)
    {
        AZ_UNUSED(oldRtpcImplData);
    }

    auto AudioSystemImpl_SteamAudio::NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
        -> Audio::IATLSwitchStateImplData*
    {
        AZ_UNUSED(audioSwitchStateNode);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioSwitchStateImplData(Audio::IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        AZ_UNUSED(oldSwitchStateImplData);
    }

    auto AudioSystemImpl_SteamAudio::NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
        -> Audio::IATLEnvironmentImplData*
    {
        AZ_UNUSED(audioEnvironmentNode);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioEnvironmentImplData(Audio::IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        AZ_UNUSED(oldEnvironmentImplData);
    }

    auto AudioSystemImpl_SteamAudio::NewGlobalAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::NewAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData*
    {
        return aznew SATLAudioObjectData_SteamAudio(objectId, false);
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioObjectData(Audio::IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData);
    }

    auto AudioSystemImpl_SteamAudio::NewDefaultAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_SteamAudio*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    auto AudioSystemImpl_SteamAudio::NewAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_SteamAudio*
    {
        AZ_UNUSED(objectId);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioListenerObjectData(Audio::IATLListenerData* const oldListenerData)
    {
        AZ_UNUSED(oldListenerData);
    }

    auto AudioSystemImpl_SteamAudio::NewAudioEventData(const Audio::TAudioEventID eventId) -> SATLEventData_SteamAudio*
    {
        AZ_UNUSED(eventId);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        AZ_UNUSED(oldEventData);
    }

    void AudioSystemImpl_SteamAudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        AZ_UNUSED(eventData);
    }

    auto AudioSystemImpl_SteamAudio::GetImplSubPath() const -> const char* const
    {
        return SteamAudioImplSubPath;
    }

    void AudioSystemImpl_SteamAudio::SetLanguage(const char* const language)
    {
        AZ_UNUSED(language);
    }

    // Functions below are only used when WWISE_RELEASE is not defined
    auto AudioSystemImpl_SteamAudio::GetImplementationNameString() const -> char const* const
    {
        return "MiniAudio-Dev";
    }

    void AudioSystemImpl_SteamAudio::GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const
    {
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_SteamAudio::GetMemoryPoolInfo() -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        return {};
    }

    auto AudioSystemImpl_SteamAudio::CreateAudioSource(const Audio::SAudioInputConfig& sourceConfig) -> bool
    {
        AZ_UNUSED(sourceConfig);
        return {};
    }

    void AudioSystemImpl_SteamAudio::DestroyAudioSource(Audio::TAudioSourceId sourceId)
    {
        AZ_UNUSED(sourceId);
    }

    void AudioSystemImpl_SteamAudio::SetPanningMode(Audio::PanningMode mode)
    {
        AZ_UNUSED(mode);
    }
} // namespace SteamAudio
