#include "Engine/MiniAudioEngine.h"

#include "AudioFileUtils.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/OpenMode.h"
#include "AzCore/Interface/Interface.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/Utils/Utils.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "Clients/AudioEventBus.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "MiniAudio/MiniAudioBus.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/AudioEngineEventBus.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineBus.h"

namespace BopAudio
{
    AZ_RTTI_NO_TYPE_INFO_IMPL(MiniAudioEngine, SoundEngineRequests);
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        MiniAudioEngine, "MiniAudioEngine", "{B959D7B7-CDC2-4829-B3FB-F34F4E82339E}");

    namespace Internal
    {
        auto LoadSoundBankByAssetId(AZ::Data::AssetId const& soundBankAssetId)
            -> AudioOutcome<AZ::Data::Asset<SoundBankAsset>>
        {
            if (!AZ::Data::AssetManager::IsReady())
            {
                AZ_Error(
                    "LoadSoundBankByAssetId",
                    AZ::Data::AssetManager::IsReady(),
                    "Asset Manager isn't ready!");

                return {};
            }

            return AZ::Data::AssetManager::Instance().GetAsset<SoundBankAsset>(
                soundBankAssetId, AZ::Data::AssetLoadBehavior::Default);
        }

        auto LoadSoundBankByAssetId(AZ::IO::PathView soundBankPath)
            -> AudioOutcome<AZ::Data::Asset<SoundBankAsset>>
        {
            auto const soundBankAssetId{ FindAssetId(
                soundBankPath, AZ::AzTypeInfo<SoundBankAsset>::Uuid()) };

            if (!soundBankAssetId.IsValid())
            {
                return AZ::Failure(AZStd::string::format(
                    "Failed to find the asset id for asset at '%s'",
                    soundBankPath.Native().data()));
            }

            return LoadSoundBankByAssetId(soundBankAssetId);
        }

        auto BuildBankProductPath(AZStd::string_view filename) -> AZ::IO::Path
        {
            return GetBankCachePath() / filename;
        }

        auto BuildBankProjectPath(AZ::IO::PathView filename) -> AZ::IO::Path
        {
            return AZ::IO::Path{ AZ::Utils::GetProjectPath() } / filename;
        }

        auto LoadBankToBuffer(AZ::IO::PathView absoluteBankPath) -> AZStd::vector<char>
        {
            AZ::IO::LocalFileIO fs{};
            AZ::IO::HandleType bankFileHandle{};

            auto const openResult = fs.Open(
                absoluteBankPath.Native().data(), AZ::IO::OpenMode::ModeRead, bankFileHandle);

            if (openResult != AZ::IO::ResultCode::Success)
            {
                AZ_Error(
                    "MiniAudioEngine",
                    false,
                    "Failed to open init soundbank file at '%s'",
                    absoluteBankPath.Native().data());

                return {};
            }

            AZ::u64 const bufferSize = [&]() -> decltype(bufferSize)
            {
                auto size{ decltype(bufferSize){} };
                fs.Size(bankFileHandle, size);

                return size;
            }();

            AZStd::vector<char> tempBuffer(bufferSize);

            auto const readResult = fs.Read(bankFileHandle, tempBuffer.data(), tempBuffer.size());
            fs.Close(bankFileHandle);

            if (readResult != AZ::IO::ResultCode::Success)
            {
                AZ_Error(
                    "MiniAudioEngine",
                    false,
                    "Failed to read init soundbank file at '%s'",
                    absoluteBankPath.Native().data());

                tempBuffer.clear();
            }

            return tempBuffer;
        };

        auto EventHandlerExists(AudioEventId const& eventId) -> bool
        {
            return MiniAudioEventRequestBus::FindFirstHandler(eventId) != nullptr;
        }
    } // namespace Internal

    class AudioSystemImpl_miniaudio;

    MiniAudioEngine::~MiniAudioEngine()
    {
        AZLOG_INFO("Begin audio object cleanup...");
        m_audioObjects.clear();

        AZLOG_INFO("Finished audio object cleanup.");
    }

    auto MiniAudioEngine::Initialize() -> NullOutcome
    {
        AZ_Error(
            "MiniAudioEngine", AZ::Data::AssetManager::IsReady(), "Asset Manager isn't ready!");

        LoadEvents();

        m_isInit = true;
        return AZ::Success();
    }

    auto MiniAudioEngine::Shutdown() -> NullOutcome
    {
        if (!m_isInit)
        {
            return AZ::Success();
        }

        m_controlEventMap.clear();
        m_audioObjects.clear();
        m_eventAssets.clear();
        m_loadedSources.clear();
        m_isInit = false;

        return AZ::Success();
    }

    auto MiniAudioEngine::GetSoundEngine() -> ma_engine*
    {
        return MiniAudio::MiniAudioInterface::Get()->GetSoundEngine();
    }

    auto MiniAudioEngine::RegisterFile(RegisterFileData const& fileData) -> NullOutcome
    {
        AZ_Warning(
            "MiniAudioEngine",
            false,
            "RegisterFile(filename: '%s') is not yet implemented",
            fileData.m_fileName.data());

        return AZ::Success();
    }

    void MiniAudioEngine::LoadSounds()
    {
        auto* const audioSystem{ AZ::Interface<Audio::IAudioSystem>::Get() };

        if (!audioSystem)
        {
            return;
        }

        AZ::IO::Path const banksProductPath = GetBankCachePath();
        auto foundFiles = Audio::FindFilesInPath(banksProductPath.c_str(), "*.miniaudio");

        AZStd::ranges::for_each(
            foundFiles,
            [this](auto const& sourceFilename)
            {
                Audio::SAudioInputConfig inputConfig{};
                inputConfig.m_autoUnloadFile = true;
                inputConfig.m_sourceFilename = sourceFilename.c_str();
                inputConfig.m_sourceType = Audio::AudioInputSourceType::WavFile;
                inputConfig.m_numChannels = DefaultAudioChannels;
                inputConfig.m_bitsPerSample = DefaultBitsPerSample;
                inputConfig.m_sampleType = Audio::AudioInputSampleType::Float;
                inputConfig.m_autoUnloadFile = true;

                Audio::TAudioSourceId sourceId =
                    AZ::Interface<Audio::IAudioSystem>::Get()->CreateAudioSource(inputConfig);

                m_loadedSources.insert(sourceId);
                // Audio::SAudioSourceInfo audioData{};
            });
    }

    void MiniAudioEngine::LoadEvents()
    {
        auto const foundFiles{ Audio::FindFilesInPath(EventsAlias, "*.audioevent") };
        AZLOG_INFO("MiniAudioEngine found '%zu' audio event files.", foundFiles.size());

        AZStd::ranges::for_each(
            foundFiles,
            [this](auto const& filepath)
            {
                static constexpr auto audioEventAssetTypeInfo{ AZ::AzTypeInfo<AudioEventAsset>{} };

                AZ::Data::AssetId eventAssetId{};
                AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                    eventAssetId,
                    &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
                    filepath.c_str(),
                    audioEventAssetTypeInfo.Uuid(),
                    true);

                auto audioEventAsset{ AZ::Data::AssetManager::Instance().GetAsset<AudioEventAsset>(
                    eventAssetId, AZ::Data::AssetLoadBehavior::Default) };

                AZ_Error(
                    "MiniAudioEngine",
                    eventAssetId.IsValid(),
                    "Failed to find AssetId for [%s]. Id resolved to [%s].",
                    filepath.c_str(),
                    eventAssetId.ToFixedString().c_str());

                audioEventAsset.BlockUntilLoadComplete();

                // audioEventAsset->RegisterAudioEvent();

                AZ_Error(
                    "MiniAudioEngine",
                    MiniAudioEventRequestBus::HasHandlers(audioEventAsset->GetEventId()),
                    "Audio event asset [%s] is not connected to its bus.",
                    filepath.c_str());

                AZ_Error(
                    "MiniAudioEngine",
                    audioEventAsset.IsReady(),
                    "Failed to load audio event [%s]. Hint: [%s].",
                    filepath.c_str(),
                    audioEventAsset.GetHint().c_str());

                if (audioEventAsset.IsReady())
                {
                    m_eventAssets.emplace_back(AZStd::move(audioEventAsset));
                }
            });
    }

    auto MiniAudioEngine::StartEvent(StartEventData const& startEventRequest) -> NullOutcome
    {
        AudioObject* const audioObj = [&startEventRequest, this]() -> decltype(audioObj)
        {
            auto targetAudioObject{ FindAudioObject(startEventRequest.m_audioObjectId) };

            return targetAudioObject ? targetAudioObject : &m_globalObject;
        }();

        NullOutcome tryStartEventResult =
            [&startEventRequest, this, &audioObj]() -> decltype(tryStartEventResult)
        {
            if (!MiniAudioEventRequestBus::HasHandlers(startEventRequest.m_audioEventId))
            {
                return AZ::Failure("Failed to start the event because there is no event connected "
                                   "with the given id.");
            }

            bool result{ true };
            MiniAudioEventRequestBus::EventResult(
                result,
                startEventRequest.m_audioEventId,
                &MiniAudioEventRequests::TryStartEvent,
                (audioObj != nullptr) ? *audioObj : m_globalObject);

            return AZ::Failure("The event indicated that it failed ");
        }();

        if (!tryStartEventResult)
        {
            AZ_Error("MiniAudioEngine", false, tryStartEventResult.GetError().c_str());
            return tryStartEventResult;
        }
        AZ_Error("MiniAudioEngine", false, "????");
        return AZ::Success();
    }

    auto MiniAudioEngine::StopEvent([[maybe_unused]] AudioEventId eventId) -> NullOutcome
    {
        if (MiniAudioEventRequestBus::HasHandlers(eventId))
        {
            return AZ::Failure(
                "Failed to stop the event because there is no event connected with the given id.");
        }

        bool const result = [&eventId, this]() -> decltype(result)
        {
            bool callResult{};
            MiniAudioEventRequestBus::EventResult(
                callResult,
                eventId,
                &MiniAudioEventRequestBus::Events::TryStopEvent,
                m_globalObject);

            return callResult;
        }();

        if (!result)
        {
            AZ::Failure("The call to the event's stop function failed.");
        }

        return AZ::Success();
    }

    auto MiniAudioEngine::CreateAudioObject() -> AudioObjectId
    {
        return m_audioObjects.emplace_back().GetId();
    }

    void MiniAudioEngine::RemoveAudioObject(AudioObjectId targetAudioObjectId)
    {
        AZStd::remove_if(
            AZStd::move_iterator(m_audioObjects.begin()),
            AZStd::move_iterator(m_audioObjects.end()),
            [targetAudioObjectId](auto const& audioObject) -> bool
            {
                return audioObject.GetId() == targetAudioObjectId;
            });
    }

    auto MiniAudioEngine::FindAudioObject(AudioObjectId targetAudioObjectId) -> AudioObject*
    {
        auto iter = AZStd::ranges::find_if(
            m_audioObjects,
            [&targetAudioObjectId](auto const& audioObject) -> bool
            {
                return audioObject.GetId() == targetAudioObjectId;
            });

        return (iter != AZStd::end(m_audioObjects)) ? iter : nullptr;
    }

    auto MiniAudioEngine::LoadSound(SoundRef const& resourceRef) -> NullOutcome
    {
        return AZ::Failure("Unimplemented.");
    }

} // namespace BopAudio
