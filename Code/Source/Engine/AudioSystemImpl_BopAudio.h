#pragma once

#include <AzCore/Component/Entity.h>
#include <AzCore/IO/Path/Path.h>

#include "ATLEntityData.h"
#include "AudioAllocators.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystemImplementation.h"

namespace BopAudio
{
    class AudioSystemImpl_bopaudio : public Audio::AudioSystemImplementation
    {
    public:
        AUDIO_IMPL_CLASS_ALLOCATOR(AudioSystemImpl_bopaudio);
        AZ_DISABLE_COPY_MOVE(AudioSystemImpl_bopaudio);
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioSystemImpl_bopaudio);

        static constexpr auto ImplName{ "BopAudio" };

        explicit AudioSystemImpl_bopaudio();
        ~AudioSystemImpl_bopaudio() override;

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

        auto RegisterAudioObject(
            Audio::IATLAudioObjectData* const audioObjectData,
            char const* const objectName) -> Audio::EAudioRequestStatus override;
        auto UnregisterAudioObject(Audio::IATLAudioObjectData* const audioObjectData)
            -> Audio::EAudioRequestStatus override;
        auto ResetAudioObject(Audio::IATLAudioObjectData* const audioObjectData)
            -> Audio::EAudioRequestStatus override;
        auto UpdateAudioObject(Audio::IATLAudioObjectData* const audioObjectData)
            -> Audio::EAudioRequestStatus override;

        auto PrepareTriggerSync(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLTriggerImplData const* const triggerData)
            -> Audio::EAudioRequestStatus override;
        auto UnprepareTriggerSync(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLTriggerImplData const* const triggerData)
            -> Audio::EAudioRequestStatus override;
        auto PrepareTriggerAsync(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLTriggerImplData const* const triggerData,
            Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus override;
        auto UnprepareTriggerAsync(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLTriggerImplData const* const triggerData,
            Audio::IATLEventData* const eventData) -> Audio::EAudioRequestStatus override;
        auto ActivateTrigger(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLTriggerImplData const* const triggerData,
            Audio::IATLEventData* const eventData,
            Audio::SATLSourceData const* const pSourceData) -> Audio::EAudioRequestStatus override;
        auto StopEvent(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLEventData const* const eventData) -> Audio::EAudioRequestStatus override;
        auto StopAllEvents(Audio::IATLAudioObjectData* const audioObjectData)
            -> Audio::EAudioRequestStatus override;
        auto SetPosition(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::SATLWorldPosition const& worldPosition) -> Audio::EAudioRequestStatus override;
        auto SetMultiplePositions(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::MultiPositionParams const& multiPositionParams)
            -> Audio::EAudioRequestStatus override;
        auto SetEnvironment(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLEnvironmentImplData const* const environmentData,
            float const amount) -> Audio::EAudioRequestStatus override;
        auto SetRtpc(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLRtpcImplData const* const rtpcData,
            float const value) -> Audio::EAudioRequestStatus override;
        auto SetSwitchState(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLSwitchStateImplData const* const switchStateData)
            -> Audio::EAudioRequestStatus override;
        auto SetObstructionOcclusion(
            Audio::IATLAudioObjectData* const audioObjectData,
            float const obstruction,
            float const occlusion) -> Audio::EAudioRequestStatus override;
        auto SetListenerPosition(
            Audio::IATLListenerData* const listenerData,
            Audio::SATLWorldPosition const& newPosition) -> Audio::EAudioRequestStatus override;
        auto ResetRtpc(
            Audio::IATLAudioObjectData* const audioObjectData,
            Audio::IATLRtpcImplData const* const rtpcData) -> Audio::EAudioRequestStatus override;

        auto RegisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry)
            -> Audio::EAudioRequestStatus override;
        auto UnregisterInMemoryFile(Audio::SATLAudioFileEntryInfo* const audioFileEntry)
            -> Audio::EAudioRequestStatus override;

        auto ParseAudioFileEntry(
            const AZ::rapidxml::xml_node<char>* audioFileEntryNode,
            Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
            -> Audio::EAudioRequestStatus override;
        void DeleteAudioFileEntryData(
            Audio::IATLAudioFileEntryData* const oldAudioFileEntryData) override;
        auto GetAudioFileLocation(Audio::SATLAudioFileEntryInfo* const fileEntryInfo)
            -> char const* const override;

        auto NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode)
            -> Audio::IATLTriggerImplData* override;
        void DeleteAudioTriggerImplData(
            Audio::IATLTriggerImplData* const oldTriggerImplData) override;

        auto NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode)
            -> Audio::IATLRtpcImplData* override;
        void DeleteAudioRtpcImplData(Audio::IATLRtpcImplData* const oldRtpcImplData) override;

        auto NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
            -> Audio::IATLSwitchStateImplData* override;
        void DeleteAudioSwitchStateImplData(
            Audio::IATLSwitchStateImplData* const oldSwitchStateImplData) override;

        auto NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
            -> Audio::IATLEnvironmentImplData* override;
        void DeleteAudioEnvironmentImplData(
            Audio::IATLEnvironmentImplData* const oldEnvironmentImplData) override;

        auto NewGlobalAudioObjectData(Audio::TAudioObjectID const objectId)
            -> Audio::IATLAudioObjectData* override;
        auto NewAudioObjectData(Audio::TAudioObjectID const objectId)
            -> Audio::IATLAudioObjectData* override;
        void DeleteAudioObjectData(Audio::IATLAudioObjectData* const oldObjectData) override;

        auto NewDefaultAudioListenerObjectData(Audio::TATLIDType const objectId)
            -> Audio::IATLListenerData* override;
        auto NewAudioListenerObjectData(Audio::TATLIDType const objectId)
            -> Audio::IATLListenerData* override;
        void DeleteAudioListenerObjectData(Audio::IATLListenerData* const oldListenerData) override;

        auto NewAudioEventData(Audio::TAudioEventID const eventId)
            -> Audio::IATLEventData* override;
        void DeleteAudioEventData(Audio::IATLEventData* const oldEventData) override;
        void ResetAudioEventData(Audio::IATLEventData* const eventData) override;

        [[nodiscard]] auto GetImplSubPath() const -> char const* const override;
        void SetLanguage(char const* const language) override;

        // Functions below are only used when RELEASE is not defined
        [[nodiscard]] auto GetImplementationNameString() const -> char const* const override;
        void GetMemoryInfo(Audio::SAudioImplMemoryInfo& memoryInfo) const override;
        auto GetMemoryPoolInfo() -> AZStd::vector<Audio::AudioImplMemoryPoolInfo> override;

        auto CreateAudioSource(Audio::SAudioInputConfig const& sourceConfig) -> bool override;
        void DestroyAudioSource(Audio::TAudioSourceId sourceId) override;

        void SetPanningMode(Audio::PanningMode mode) override;
        //////////////////////////////////////////////////////////////////////////

    private:
        static char const* const BopAudioImplSubPath;
        AZStd::string m_language{};

        Audio::PanningMode m_panningMode{ Audio::PanningMode::Speakers };

        AZ::IO::Path m_soundBankFolder{};
        AZ::IO::Path m_localizedSoundBankFolder{};
        AZStd::unordered_set<Audio::IATLAudioObjectData*> m_registeredObjects{};
    };
} // namespace BopAudio
