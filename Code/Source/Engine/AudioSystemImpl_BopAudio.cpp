#include "Engine/AudioSystemImpl_BopAudio.h"



#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"

#include "BopAudio/AudioAsset.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/ConfigurationSettings.h"

#include "AzCore/PlatformDef.h"
AZ_PUSH_DISABLE_WARNING(4701 4244 4456 4457 4245, "-Wuninitialized") 

extern "C" {

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>    // Enables Vorbis decoding.

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

// The stb_vorbis implementation must come after the implementation of miniaudio.
#undef STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

}

AZ_POP_DISABLE_WARNING


namespace BopAudio
{
    char const* const AudioSystemImpl_BopAudio::BopAudioImplSubPath = "bopaudio/";

    AudioSystemImpl_BopAudio::AudioSystemImpl_BopAudio(AZStd::string_view assetsPlatformName)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(serializeContext, &AZ::ComponentApplicationBus::Events::GetSerializeContext);
        m_audioEntityContext = AZStd::make_unique<AzFramework::EntityContext>(serializeContext);

        // MiniAudio::MiniAudioRequestBus::BroadcastResult(m_engine, &MiniAudio::MiniAudioRequests::GetSoundEngine);

        /*
              if (!m_engine)
              {
                  AZLOG_ERROR("Failed to get the MiniAudio engine!");
              }
              */

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
                AZLOG_INFO("InitLibraryPath set: %s.", initLibraryPath.c_str());
                // "sounds/bopaudio/linux/init.soundlib"
                AZ::StringFunc::AssetDatabasePath::Join(platformPath.c_str(), InitLibrary, initLibraryPath);
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
            AZLOG_ERROR("Failed to find Bop Audio configuration file: \"%s\".", configFilename.c_str()); // NOLINT
        }

        m_soundLibraryFolder = libraryPath;
        m_localizedSoundLibraryFolder = libraryPath;

        SetLibrariesRootPath(m_soundLibraryFolder);
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
        m_audioEntityContext->InitContext();

        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AudioAsset>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(&AZ::Data::AssetCatalogRequests::AddExtension, AudioAsset::FileExtension);

        AZLOG_INFO("Bop Audio System Implementation initialized."); // NOLINT
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ShutDown() -> Audio::EAudioRequestStatus
    {
        m_audioEntityContext->DestroyContext();
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

        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnregisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "UnregisterAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "ResetAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UpdateAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "UpdateAudioObject.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::PrepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const /*triggerData*/)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "PrepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }
    auto AudioSystemImpl_BopAudio::UnprepareTriggerSync(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "UnprepareTriggerSync.\n");
        AZ_UNUSED(audioObjectData, triggerData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::PrepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "PrepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnprepareTriggerAsync(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "UnprepareTriggerAsync.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ActivateTrigger(
        Audio::IATLAudioObjectData* const audioObjectData,
        const Audio::IATLTriggerImplData* const triggerData,
        Audio::IATLEventData* const eventData,
        const Audio::SATLSourceData* const pSourceData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "ActivateTrigger.\n");
        AZ_UNUSED(audioObjectData, triggerData, eventData, pSourceData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::StopEvent(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEventData* const eventData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "StopEvent.\n");
        AZ_UNUSED(audioObjectData, eventData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "StopAllEvents.\n");
        AZ_UNUSED(audioObjectData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetPosition(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::SATLWorldPosition& worldPosition) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetPosition.\n");
        AZ_UNUSED(audioObjectData, worldPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetMultiplePositions(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::MultiPositionParams& multiPositionParams)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetMultiplePositions.\n");
        AZ_UNUSED(audioObjectData, multiPositionParams);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetEnvironment(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEnvironmentImplData* const environmentData, const float amount)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetEnvironment.\n");
        AZ_UNUSED(audioObjectData, environmentData, amount);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData, const float value)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData, value);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetSwitchState(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLSwitchStateImplData* const switchStateData)
        -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetSwitchState.\n");
        AZ_UNUSED(audioObjectData, switchStateData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetObstructionOcclusion(
        Audio::IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "SetObstructionOcclusion.\n");
        AZ_UNUSED(audioObjectData, obstruction, occlusion);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::SetListenerPosition(
        Audio::IATLListenerData* const listenerData, const Audio::SATLWorldPosition& newPosition) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(listenerData, newPosition);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ResetRtpc(
        Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "ResetRtpc.\n");
        AZ_UNUSED(audioObjectData, rtpcData);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::RegisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZLOG(ASI_BopAudio, "RegisterInMemoryFile.\n");
        AZ_UNUSED(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::UnregisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntry);
        return Audio::EAudioRequestStatus::Success;
    }

    auto AudioSystemImpl_BopAudio::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode, Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
        -> Audio::EAudioRequestStatus
    {
        AZ_UNUSED(audioFileEntryNode, fileEntryInfo);
        // TODO: Implement 1/22/24
        return Audio::EAudioRequestStatus::Success;
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
        AZLOG(ASI_BopAudio, "NewAudioEventData");
        auto& eventDataPtr{ m_audioEvents.emplace_back(AZStd::make_unique<SATLEventData_BopAudio>(Audio::EAudioEventState::eAES_NONE)) };

        return eventDataPtr.get();
    }

    void AudioSystemImpl_BopAudio::DeleteAudioEventData(Audio::IATLEventData* const oldEventData)
    {
        auto foundIter = AZStd::ranges::find_if(
            m_audioEvents,
            [&](auto const& eventDataPtr) -> bool
            {
                return eventDataPtr.get() == oldEventData;
            });

        if (foundIter == m_audioEvents.end())
        {
            return;
        }

        m_audioEvents.erase(foundIter);
    }

    void AudioSystemImpl_BopAudio::ResetAudioEventData(Audio::IATLEventData* const eventData)
    {
        eventData->m_owner = nullptr;
        eventData->m_triggerId = INVALID_AUDIO_CONTROL_ID;

        if (auto* bopEventData{ azrtti_cast<SATLEventData_BopAudio*>(eventData) })
        {
            bopEventData->m_audioEventState = Audio::EAudioEventState::eAES_NONE;
            bopEventData->m_sourceId = INVALID_AUDIO_CONTROL_ID;
            bopEventData->m_maId = AZ::Crc32();
        }
    }

    auto AudioSystemImpl_BopAudio::GetImplSubPath() const -> const char* const
    {
        return BopAudioImplSubPath;
    }

    void AudioSystemImpl_BopAudio::SetLanguage(const char* const language)
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
        AZ_UNUSED(memoryInfo);
    }

    auto AudioSystemImpl_BopAudio::GetMemoryPoolInfo() -> AZStd::vector<Audio::AudioImplMemoryPoolInfo>
    {
        return {};
    }

    auto AudioSystemImpl_BopAudio::CreateAudioSource(const Audio::SAudioInputConfig& sourceConfig) -> bool
    {
        AZLOG(ASI_BopAudio, "CreateAudioSource");

        ma_sound sound{};
        auto result = ma_sound_get_min_gain(&sound);

        if (result != MA_SUCCESS)
        {
            AZLOG(ASI_BopAudio, "CreateAudioSource: Failed to load audio source.");
            return false;
        }

        return true;
    }

    void AudioSystemImpl_BopAudio::DestroyAudioSource(Audio::TAudioSourceId sourceId)
    {
        AZ_UNUSED(sourceId);
    }

    void AudioSystemImpl_BopAudio::SetPanningMode(Audio::PanningMode mode)
    {
        m_panningMode = mode;
    }
} // namespace BopAudio
