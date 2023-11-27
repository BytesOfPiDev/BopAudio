#pragma once

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "AzCore/Asset/AssetManager.h"
#include "Engine/ATLEntities_SteamAudio.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"
#include "phonon.h"

namespace SteamAudio
{

    class AudioSystemImpl_SteamAudio : public Audio::AudioSystemImplementation
    {
    public:
        AUDIO_IMPL_CLASS_ALLOCATOR(AudioSystemImpl_SteamAudio); // NOLINT
        AZ_DISABLE_COPY_MOVE(AudioSystemImpl_SteamAudio); // NOLINT

        explicit AudioSystemImpl_SteamAudio(AZStd::string_view assetsPlatformName);
        ~AudioSystemImpl_SteamAudio() override;

        void SetPaths();

        void OnAudioSystemLoseFocus() override;
        void OnAudioSystemGetFocus() override;
        void OnAudioSystemMuteAll() override;
        void OnAudioSystemUnmuteAll() override;
        void OnAudioSystemRefresh() override;

        //////////////////////////////////////////////////////////////////////////
        // AudioSystemImplementationRequestBus
        void Update(float const updateIntervalMS) override;

        auto Initialize() -> Audio::EAudioRequestStatus override;
        auto ShutDown() -> Audio::EAudioRequestStatus override;
        auto Release() -> Audio::EAudioRequestStatus override;

        auto StopAllSounds() -> Audio::EAudioRequestStatus override;

        auto RegisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData, const char* const objectName)
            -> Audio::EAudioRequestStatus override;
        auto UnregisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus override;
        auto ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus override;
        auto UpdateAudioObject(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus override;

        auto PrepareTriggerSync(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
            -> Audio::EAudioRequestStatus override;
        auto UnprepareTriggerSync(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLTriggerImplData* const triggerData)
            -> Audio::EAudioRequestStatus override;
        auto PrepareTriggerAsync(
            Audio::IATLAudioObjectData* const audioObjectData,
            const Audio::IATLTriggerImplData* const triggerData,
            Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus override;
        auto UnprepareTriggerAsync(
            Audio::IATLAudioObjectData* const audioObjectData,
            const Audio::IATLTriggerImplData* const triggerData,
            Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus override;
        auto ActivateTrigger(
            Audio::IATLAudioObjectData* const audioObjectData,
            const Audio::IATLTriggerImplData* const triggerData,
            Audio::IATLEventData* const eventData,
            const Audio::SATLSourceData* const pSourceData) -> Audio::EAudioRequestStatus override;
        auto StopEvent(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLEventData* const eventData)
            -> Audio::EAudioRequestStatus override;
        auto StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData) -> Audio::EAudioRequestStatus override;
        auto SetPosition(Audio::IATLAudioObjectData* const audioObjectData, const Audio::SATLWorldPosition& worldPosition)
            -> Audio::EAudioRequestStatus override;
        auto SetMultiplePositions(Audio::IATLAudioObjectData* const audioObjectData, const Audio::MultiPositionParams& multiPositionParams)
            -> Audio::EAudioRequestStatus override;
        auto SetEnvironment(
            Audio::IATLAudioObjectData* const audioObjectData,
            const Audio::IATLEnvironmentImplData* const environmentData,
            const float amount) -> Audio::EAudioRequestStatus override;
        auto SetRtpc(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData, const float value)
            -> Audio::EAudioRequestStatus override;
        auto SetSwitchState(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLSwitchStateImplData* const switchStateData)
            -> Audio::EAudioRequestStatus override;
        auto SetObstructionOcclusion(Audio::IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion)
            -> Audio::EAudioRequestStatus override;
        auto SetListenerPosition(Audio::IATLListenerData* const listenerData, const Audio::SATLWorldPosition& newPosition)
            -> Audio::EAudioRequestStatus override;
        auto ResetRtpc(Audio::IATLAudioObjectData* const audioObjectData, const Audio::IATLRtpcImplData* const rtpcData)
            -> Audio::EAudioRequestStatus override;

        auto RegisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus override;
        auto UnregisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry) -> Audio::EAudioRequestStatus override;

        auto ParseAudioFileEntry(const AZ::rapidxml::xml_node<char>* audioFileEntryNode, Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
            -> Audio::EAudioRequestStatus override;
        void DeleteAudioFileEntryData(Audio::IATLAudioFileEntryData* const oldAudioFileEntryData) override;
        auto GetAudioFileLocation(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> const char* const override;

        auto NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode) -> Audio::IATLTriggerImplData* override;
        void DeleteAudioTriggerImplData(Audio::IATLTriggerImplData* const oldTriggerImplData) override;

        auto NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode) -> Audio::IATLRtpcImplData* override;
        void DeleteAudioRtpcImplData(Audio::IATLRtpcImplData* const oldRtpcImplData) override;

        auto NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
            -> Audio::IATLSwitchStateImplData* override;
        void DeleteAudioSwitchStateImplData(Audio::IATLSwitchStateImplData* const oldSwitchStateImplData) override;

        auto NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
            -> Audio::IATLEnvironmentImplData* override;
        void DeleteAudioEnvironmentImplData(Audio::IATLEnvironmentImplData* const oldEnvironmentImplData) override;

        auto NewGlobalAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData* override;
        auto NewAudioObjectData(const Audio::TAudioObjectID objectId) -> Audio::IATLAudioObjectData* override;
        void DeleteAudioObjectData(Audio::IATLAudioObjectData* const oldObjectData) override;

        auto NewDefaultAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_SteamAudio* override;
        auto NewAudioListenerObjectData(const Audio::TATLIDType objectId) -> SATLListenerData_SteamAudio* override;
        void DeleteAudioListenerObjectData(Audio::IATLListenerData* const oldListenerData) override;

        auto NewAudioEventData(const Audio::TAudioEventID eventId) -> SATLEventData_SteamAudio* override;
        void DeleteAudioEventData(Audio::IATLEventData* const oldEventData) override;
        void ResetAudioEventData(Audio::IATLEventData* const eventData) override;

        [[nodiscard]] auto GetImplSubPath() const -> const char* const override;
        void SetLanguage(const char* const language) override;

        // Functions below are only used when WWISE_RELEASE is not defined
        [[nodiscard]] auto GetImplementationNameString() const -> const char* const override;
        void GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const override;
        auto GetMemoryPoolInfo() -> AZStd::vector<Audio::AudioImplMemoryPoolInfo> override;

        auto CreateAudioSource(const Audio::SAudioInputConfig& sourceConfig) -> bool override;
        void DestroyAudioSource(Audio::TAudioSourceId sourceId) override;

        void SetPanningMode(Audio::PanningMode mode) override;
        //////////////////////////////////////////////////////////////////////////

    private:
        static char const* const SteamAudioImplSubPath;
        AZStd::string m_assetsPlatform;
        // ma_engine m_engine;

        IPLContextSettings m_contextSettings{};
        IPLContext m_context{ nullptr };
        IPLHRTFSettings m_hrtfSettings{};
        IPLAudioSettings m_audioSettings{};
        IPLHRTF m_hrtf = nullptr;

        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers;

        AZStd::string m_soundLibraryFolder{};
        AZStd::string m_localizedSoundLibraryFolder{};
    };
} // namespace SteamAudio
