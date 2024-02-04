#include "Engine/MiniAudioEngine.h"

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/OpenMode.h"
#include "AzCore/Serialization/Utils.h"
#include "AzCore/Utils/Utils.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "MiniAudio/MiniAudioBus.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Clients/SoundBankAsset.h"
#include "Engine/AudioObject.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/MiniAudioEngineRequests.h"
#include "Engine/MiniAudioIncludes.h"

namespace BopAudio
{
    namespace Internal
    {
        auto GetSoundBankAssetId(AZ::IO::PathView soundBankPath) -> AZ::Data::AssetId
        {
            auto result = decltype(GetSoundBankAssetId(soundBankPath)){};

            AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                result,
                &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
                soundBankPath.Native().data(),
                AZ::AzTypeInfo<SoundBankAsset>::Uuid(),
                false);

            return result;
        }

        auto BuildBankCachePath(AZStd::string_view fileName) -> AZ::IO::Path
        {
            AZ::IO::Path builtPath{ AZ::Utils::GetProjectProductPathForPlatform() };
            builtPath /= DefaultBanksPath;
            builtPath /= fileName;

            return builtPath;
        }

        auto LoadBankToMemory(AZ::IO::PathView absoluteBankPath) -> AZStd::vector<char>
        {
            // We can't use the asset manager to get the init soundbank because we're loaded early
            // in O3DE's bootstrapping process. We use a file stream to get the file.
            AZ::IO::LocalFileIO fs{};
            AZ::IO::HandleType initBankFileHandle{};

            auto const openResult = fs.Open(
                absoluteBankPath.Native().data(), AZ::IO::OpenMode::ModeRead, initBankFileHandle);

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
                fs.Size(initBankFileHandle, size);

                return size;
            }();

            AZStd::vector<char> tempBuffer(bufferSize);

            auto const readResult =
                fs.Read(initBankFileHandle, tempBuffer.data(), tempBuffer.size());
            fs.Close(initBankFileHandle);

            if (readResult != AZ::IO::ResultCode::Success)
            {
                AZ_Error(
                    "MiniAudioEngine",
                    false,
                    "Failed to read init soundbank file at '%s'",
                    absoluteBankPath.Native().data());

                return {};
            }

            return tempBuffer; // TODO: Research C++ RVO
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
        AZ::IO::Path const initBankPath = Internal::BuildBankCachePath(InitBank);

        auto const initBankBuffer{ Internal::LoadBankToMemory(initBankPath) };

        if (initBankBuffer.empty())
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to read init soundbank file at path '%s'", initBankPath.c_str()));
        }

        m_initSoundBank.reset(AZ::Utils::LoadObjectFromBuffer<SoundBankAsset>(
            initBankBuffer.data(), initBankBuffer.size()));

        if (!m_initSoundBank)
        {
            return AZ::Failure("Failed to load the init bank");
        }

        return AZ::Success();
    }

    auto MiniAudioEngine::Shutdown() -> bool
    {
        m_soundBanks.clear();

        return true;
    }

    auto MiniAudioEngine::GetSoundEngine() -> ma_engine*
    {
        return MiniAudio::MiniAudioInterface::Get()->GetSoundEngine();
    }

    auto MiniAudioEngine::LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileInfo)
        -> NullOutcome
    {
        if (!fileInfo)
        {
            return AZ::Failure("Argument is a null pointer.");
        }

        AZStd::span<char> buffer{ static_cast<char*>(fileInfo->pFileData), fileInfo->nSize };

        auto loadedBank = decltype(m_soundBanks)::node_type{
            AZ::Utils::LoadObjectFromBuffer<SoundBankAsset>(buffer.data(), buffer.size())
        };

        if (loadedBank == nullptr)
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to load sound bank '%s' from pre-loaded buffer", fileInfo->sFileName));
        }

        m_soundBanks.emplace_back(AZStd::move(loadedBank));

        return AZ::Success();
    }

    void MiniAudioEngine::LoadTrigger(AZ::rapidxml::xml_node<char>* triggerNode)
    {
        if (!triggerNode)
        {
            return;
        }
    }

    auto MiniAudioEngine::ActivateTrigger(ActivateTriggerRequest const& activateTriggerRequest)
        -> AudioOutcome<void>
    {
        // We need an audio object to activate the trigger/event on. The request should have
        // provided one.
        auto* audioObject{ FindAudioObject(activateTriggerRequest.m_audioObjectId) };
        // For now, we'll create the audio object if it doesn't already exist, but emit a warning.
        if (!audioObject)
        {
            AZ_Warning(
                "MiniAudioEngine",
                false,
                "The given audio object does not exist, so we had to create it.");

            auto& newAudioObject = m_audioObjects.emplace_back();
            audioObject = &newAudioObject;
        }

        AudioEventAsset const& eventAsset = *activateTriggerRequest.m_eventAsset.Get();
        eventAsset(*audioObject);

        /*
              if (auto createEventOutcome{ CreateEvent(activateTriggerRequest.m_triggerResource) })
              {
              }
              else
              {
                  return AZ::Failure(AZStd::string::format(
                      "Failed to create event with resource ['%s'].",
                      activateTriggerRequest.m_triggerResource.GetCStr()));
              }
              */

        return AZ::Success();
    }

    auto MiniAudioEngine::CreateAudioObject(UniqueId const&) -> bool
    {
        return true;
    }

    void MiniAudioEngine::RemoveAudioObject(UniqueId /*audioObjectId*/)
    {
        /*
              AZStd::remove_if(
                  m_audioObjects.begin(),
                  m_audioObjects.end(),
                  [audioObjectId](auto const& audioObject) -> bool
                  {
                      return audioObject == audioObjectId;
                  });
        */
    }

    auto MiniAudioEngine::FindAudioObject(AudioObjectId /*audioObjectId*/) -> AudioObject*
    {
        /*
              auto iter = AZStd::ranges::find_if(
                  m_audioObjects,
                  [&audioObjectId](auto const& audioObject) -> bool
                  {
                      return audioObjectId == audioObject.GetUniqueId();
                  });

              return iter != AZStd::end(m_audioObjects) ? &(*iter) : nullptr;
        */
        return nullptr;
    }

    auto MiniAudioEngine::CreateEvent(AudioEventId) const -> AudioOutcome<AudioEventAsset>
    {
        return AZ::Failure("Unimplemented");
    }

    void MiniAudioEngine::PlaySound(ma_sound* soundInstance, AZ::Name const& soundName)
    {
        auto result = ma_sound_seek_to_pcm_frame(soundInstance, 0);

        AZ_Error(
            "MiniAudioEngine",
            result == MA_SUCCESS,
            "Failed to seek audio frame on sound: '%s'.",
            soundName.GetCStr());

        result = ma_sound_start(soundInstance);

        AZ_Error(
            "MiniAudioEngine",
            result == MA_SUCCESS,
            "Failed to start sound: '%s'.",
            soundName.GetCStr());
    }

} // namespace BopAudio
