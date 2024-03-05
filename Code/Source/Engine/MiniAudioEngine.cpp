#include "Engine/MiniAudioEngine.h"

#include "AudioFileUtils.h"
#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/OpenMode.h"
#include "AzCore/Interface/Interface.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/Utils/Utils.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "IAudioInterfacesCommonData.h"
#include "IAudioSystem.h"
#include "MiniAudio/MiniAudioBus.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/AudioEventBus.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineRequests.h"
#include "Engine/MiniAudioIncludes.h"
#include <AzCore/Outcome/Outcome.h>

namespace BopAudio
{
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
    } // namespace Internal
    class AudioSystemImpl_BopAudio;

    MiniAudioEngine::MiniAudioEngine() = default;

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

        static constexpr auto example_one{ "dlg/soldierintro.ogg.miniaudio" };
        auto example_one_source{ AZStd::make_unique<SoundSource>(example_one) };
        example_one_source->Load();
        m_soundSourceMap[ResourceRef{ example_one }] = AZStd::move(example_one_source);

        return AZ::Success();
    }

    auto MiniAudioEngine::Shutdown() -> bool
    {
        m_controlEventMap.clear();
        m_audioObjects.clear();

        return true;
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

        AZStd::ranges::transform(
            foundFiles,
            AZStd::inserter(m_eventAssets, m_eventAssets.end()),
            [](auto const& filepath) -> decltype(m_eventAssets)::value_type
            {
                AZ::Data::AssetId eventAssetId{};
                AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                    eventAssetId,
                    &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
                    filepath.c_str(),
                    AZ::AzTypeInfo<AudioEventAsset>::Uuid(),
                    true);

                auto audioEventAsset{ AZ::Data::AssetManager::Instance().GetAsset<AudioEventAsset>(
                    eventAssetId, AZ::Data::AssetLoadBehavior::Default) };

                audioEventAsset.BlockUntilLoadComplete();

                AZ_Warning(
                    "MiniAudioEngine",
                    audioEventAsset.IsError(),
                    "Failed to queue load event asset '%s'",
                    filepath.c_str());

                AZ_Error(
                    "MiniAudioEngine",
                    audioEventAsset.IsReady(),
                    "Failed to load asset '%s'",
                    filepath.c_str());

                return { audioEventAsset->GetId(), audioEventAsset };
            });
    }

    auto MiniAudioEngine::ActivateTrigger(ActivateEventData const& activateTriggerRequest)
        -> NullOutcome
    {
        auto const foundEvent{ m_eventAssets.find(activateTriggerRequest.m_audioEventId) };

        if (foundEvent == AZStd::end(m_eventAssets))
        {
            return AZ::Failure("Unable to find an AudioEvent with the given id");
        }

        auto const eventAsset{ FindAudioEvent(activateTriggerRequest.m_audioEventId) };

        if (!eventAsset.IsReady())
        {
            return AZ::Failure("The asset isn't ready - it should have already been pre-loaded");
        }

        AudioObject* const audioObj = [&activateTriggerRequest, this]() -> decltype(audioObj)
        {
            auto targetAudioObject{ FindAudioObject(activateTriggerRequest.m_audioObjectId) };

            return targetAudioObject ? targetAudioObject : &m_globalObject;
        }();
        if (!audioObj)
        {
            return AZ::Failure("Failed to find an audio object with the given id");
        }

        static constexpr auto executeEventOnObject = [](AudioEventAsset& eventAsset,
                                                        AudioObject& audioObject) -> void
        {
            eventAsset(audioObject);
        };

        executeEventOnObject(*eventAsset, *audioObj);

        AudioEventNotificationBus::Event(
            EventNotificationIdType(
                activateTriggerRequest.m_audioControlId, activateTriggerRequest.m_owner),
            &AudioEventNotifications::ReportEventStarted,
            EventStartedData{});

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

    auto MiniAudioEngine::FindAudioEvent(AudioEventId targetAudioEventId)
        -> AZ::Data::Asset<AudioEventAsset>
    {
        auto foundEventIter{ m_eventAssets.find(targetAudioEventId) };

        return (foundEventIter != AZStd::end(m_eventAssets))
            ? foundEventIter->second
            : decltype(m_eventAssets)::mapped_type{};
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

    auto MiniAudioEngine::LoadSound(ResourceRef const& resourceRef) -> NullOutcome
    {
        if (m_soundSourceMap.contains(resourceRef))
        {
            return AZ::Success();
        }

        // auto source{ AZStd::make_unique<SoundSource>(resourceRef) };
        // source->Load();

        // m_soundSourceMap[resourceRef] = AZStd::move(source);

        return AZ::Failure("dunno");
    }

} // namespace BopAudio
