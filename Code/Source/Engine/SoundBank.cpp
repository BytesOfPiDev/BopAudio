#include "Engine/SoundBank.h"

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/JSON/rapidjson.h>
#include <rapidjson/document.h>

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/smart_ptr/make_shared.h"
#include "Engine/DocumentReader.h"
#include "MiniAudio/SoundAsset.h"

#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"

namespace BopAudio
{
    SoundBank::SoundBank(AZStd::string_view soundBankFileName)
        : m_id{ ResourceRef(soundBankFileName) }
        , m_soundBankName{ soundBankFileName }
    {
    }

    SoundBank::SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData)
        : m_id{ ResourceRef(fileEntryData ? fileEntryData->sFileName : nullptr) }
        , m_fileEntryInfo{ fileEntryData }
        , m_soundBankName{ fileEntryData ? decltype(m_soundBankName){ fileEntryData->sFileName }
                                         : decltype(m_soundBankName){} }
    {
    }

    auto SoundBank::LoadInitBank() -> AZ::Outcome<SoundBank, char const*>
    {
        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) / InitBank };

        auto buffer{ LoadSoundBankToBuffer(banksRootPath.Native()) };

        if (SoundBank initBank{ InitBank }; initBank.Load())
        {
            return initBank;
        }

        return AZ::Failure("Failed to load the init soundbank.");
    }

    auto SoundBank::LoadSounds() -> bool
    {
        bool const isSuccess = AZStd::ranges::all_of(
            m_soundNames,
            [this](auto const& soundName) -> bool
            {
                SoundSource soundSource{ AZ::IO::Path{ soundName.GetCStr() } };
                soundSource.Load();

                if (!soundSource.IsReady())
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Failed to find sound asset '%s'.",
                        soundName.GetCStr());
                    return false;
                }

                m_soundAssets[ResourceRef{ soundName }] = soundSource;

                return true;
            });

        return isSuccess;
    }

    auto SoundBank::LoadBuffer() -> bool
    {
        // We first try to load using the buffer that the AudioSystemInterface has.
        m_bufferRef = m_fileEntryInfo
            ? decltype(m_bufferRef){ static_cast<char*>(m_fileEntryInfo->pFileData),
                                     m_fileEntryInfo->nSize }
            : decltype(m_bufferRef){};

        m_optionalBuffer.reset();

        if (m_bufferRef.empty())
        {
            // If that didn't work, we try to load the buffer using the name of the sound bank.
            // TODO: Verify:
            // This should generally only trigger for the default soundbank, since I don't think
            // that the ATL tries to load it, or, if it does, doesn't automatically pass it to the
            // ASI implementation.
            m_optionalBuffer = [this]()
            {
                auto loadOutcome{ LoadSoundBankToBuffer(
                    AZ::IO::Path{ GetBanksRootPath() } / m_soundBankName.GetCStr()) };

                return loadOutcome.IsSuccess() ? loadOutcome.TakeValue()
                                               : decltype(m_optionalBuffer){};
            }();

            m_bufferRef = m_optionalBuffer ? m_optionalBuffer.value() : decltype(m_bufferRef){};
        }

        return m_bufferRef.empty();
    }

    auto SoundBank::Load() -> AZ::Outcome<void, char const*>
    {
        LoadBuffer();

        m_doc = AZStd::make_shared<rapidjson::Document>();
        m_doc->Parse(m_bufferRef.data(), m_bufferRef.size());

        LoadSoundNames();

        LoadSounds();
        auto const loadEventsOutcome{ LoadEvents() };
        AZ_Error(
            "SoundBank",
            loadEventsOutcome.IsSuccess(),
            "Failed to fully load events. Reason: ['%s'].",
            loadEventsOutcome.GetError().c_str());

        AZ_Info("SoundBank", "Loaded '%s'.", m_soundBankName.GetCStr());

        return AZ::Success();
    }

    auto SoundBank::LoadSoundNames() -> bool
    {
        m_soundNames = GetSoundNamesFromSoundBankFile(m_bufferRef);

        return true;
    }

    auto SoundBank::GetSoundAsset(ResourceRef const& soundName) const -> MiniAudio::SoundDataAsset
    {
        auto iter{ m_soundAssets.find(soundName) };
        if (iter == AZStd::end(m_soundAssets))
        {
            return {};
        }

        auto const& [resourceName, audioSource]{ *iter };
        return audioSource.GetAsset();
    };

    auto SoundBank::LoadEvents() -> AudioOutcome<void>
    {
        auto loadEventIdsOutcome{ LoadEventIds(*m_doc.get()) };
        m_eventIds = loadEventIdsOutcome.IsSuccess() ? loadEventIdsOutcome.TakeValue()
                                                     : decltype(m_eventIds){};
        AZLOG(
            AE_miniaudio_LoadEvents,
            "[SoundBank::LoadEvents] There are [%zu] ids.",
            m_eventIds.size());

        AZ_Warning(
            "SoundBank",
            loadEventIdsOutcome.IsSuccess(),
            "No events found for SoundBank: [%s]",
            m_id.GetCStr());

        for (auto const& eventId : m_eventIds)
        {
            auto const eventPath{ AZ::IO::Path{ JsonKeys::EventsKey_O } / eventId.GetCStr() };

            AZLOG(
                AE_miniaudio_LoadEvents,
                "[SoundBank::LoadEvents] Attempting to create event: [%s].",
                eventPath.c_str());

            auto createOutcome{ AudioEvent::CreateFromSource(*m_doc.get(), eventPath) };

            // FIX: Never creates
            if (!createOutcome.IsSuccess())
            {
                AZ_Error("SoundBank", false, "Failed to create event from source.");
                continue;
            };
            m_events.emplace_back(createOutcome.TakeValue());
        }

        return AZ::Success();
    }

    auto SoundBank::CloneEvent(AudioEventId eventId) const -> AudioOutcome<AudioEvent>
    {
        AZ_Info("SoundBank", "CloneEvent(AudioEventId eventId) BEGIN");
        AZ_Info("SoundBank", "m_events size: [%zu]", m_events.size());
        auto const foundEventIter = AZStd::ranges::find_if(
            m_events,
            [&eventId](auto const& event)
            {
                AZ_Info(
                    "SoundBank",
                    "CloneEvent checking [Event: %s] FOR [Event: %s]",
                    event.GetId().GetCStr(),
                    eventId.GetCStr());
                return event.GetId() == eventId;
            });

        if (foundEventIter != AZStd::end(m_events))
        {
            return AZ::Success(*foundEventIter);
        }

        AZ_Info("SoundBank", "CloneEvent(AudioEventId eventId) END");
        return AZ::Failure(AZStd::string::format(
            "Failed to find an event with the given event id: ['%s'].", eventId.GetCStr()));
    }

    auto SoundBank::CloneEvent(ResourceRef const& resourceId) const -> AudioOutcome<AudioEvent>
    {
        return CloneEvent(AudioEventId{ resourceId.GetAsPath().Filename().Native() });
    }

    auto LoadEventIds(rapidjson::Document const& doc) -> AudioOutcome<AZStd::vector<AudioEventId>>
    {
        AZStd::vector<AudioEventId> eventIds{};

        if (doc.HasParseError())
        {
            return AZ::Failure("Document has parse error.");
        }
        rapidjson::Pointer eventsObjectJsonPtr{ JsonKeys::EventsKey_O.Native().data() };
        rapidjson::Document::ValueType const* eventsObjectValue{ rapidjson::GetValueByPointer(
            doc, eventsObjectJsonPtr) };

        if (!eventsObjectValue->IsObject())
        {
            static constexpr auto errorFormat{
                "Expected an object from key '%s', but got something else."
            };

            return AZ::Failure(
                AZStd::string::format(errorFormat, JsonKeys::EventsKey_O.Native().data()));
        }

        rapidjson::Document::ConstObject eventsObject{ eventsObjectValue->GetObject() };

        if (eventsObject.ObjectEmpty())
        {
            static constexpr auto errorFormat{ "The object at key '%s' is empty." };
            return AZ::Failure(
                AZStd::string::format(errorFormat, JsonKeys::EventsKey_O.Native().data()));
        }

        for (auto const& eventsMember : eventsObject)
        {
            AZLOG(AE_miniaudio, "\tLoading Event: '%s'.\n", eventsMember.name.GetString());

            if (!eventsMember.value.IsObject())
            {
                AZ_Error(
                    "SoundBank",
                    false,
                    "Expected an object at '%s', but got something else.",
                    eventsMember.name.GetString());

                continue;
            }

            auto const tasksMember{ eventsMember.value.FindMember("Tasks") };
            if (tasksMember == eventsMember.value.MemberEnd())
            {
                AZ_Warning(
                    "SoundBank",
                    false,
                    "Failed to find the 'Tasks' member in '%s'.",
                    eventsMember.name.GetString());

                continue;
            }

            if (!tasksMember->value.IsArray())
            {
                AZ_Error(
                    "SoundBank",
                    false,
                    "Expected an array at '%s', but got something else.",
                    tasksMember->name.GetString());

                continue;
            }

            eventIds.emplace_back(eventsMember.name.GetString());
        }

        AZ_Info("LoadEventIds", "Events found: [%i]", eventIds.size());
        return AZ::Success(eventIds);
    }

    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath)
        -> AZ::Outcome<AZStd::vector<char>, char const*>
    {
        using ReturnType = decltype(LoadSoundBankToBuffer(soundBankFilePath));

        return [&soundBankFilePath]() -> ReturnType
        {
            AZ::IO::FileIOStream fileStream{ soundBankFilePath.c_str(),
                                             AZ::IO::OpenMode::ModeRead };
            ReturnType tempBuffer(fileStream.GetLength());
            fileStream.Read(tempBuffer.GetValue().size(), tempBuffer.GetValue().data());

            return AZ::Success(tempBuffer);
        }();
    }

    auto GetSoundNamesFromSoundBankFile(AZStd::span<char const> soundBankFileBuffer) -> SoundNames
    {
        // TODO: Investigate why loading the doc from a function and moving it into 'doc' results in
        // crash.
        rapidjson::Document doc{};
        doc.Parse(soundBankFileBuffer.data(), soundBankFileBuffer.size());

        if (auto const outcome = ValidateDocument(doc); !outcome.IsSuccess())
        {
            AZ_Error(
                "SoundBank",
                false,
                "SoundBank failed validation. Reason: %s",
                outcome.GetError().c_str());

            return {};
        }
        rapidjson::Pointer const soundsObjectKeyPtr{ JsonKeys::SoundsKey_O.Native().data() };
        auto* const soundsObjectValue{ rapidjson::GetValueByPointer(doc, soundsObjectKeyPtr) };

        if (!soundsObjectValue)
        {
            AZLOG_WARN("No sounds found at ['%s'].", JsonKeys::SoundsKey_O.Native().data());
            return {};
        }

        SoundNames soundNames{};

        for (auto& object : soundsObjectValue->GetObject())
        {
            if (!object.name.IsString())
            {
                AZ_Error(
                    "GetSoundNamesFromSoundBankFile",
                    false,
                    "Unexpected value type for SoundBank '%s'. Expected a "
                    "string.",
                    object.name.GetString());
                continue;
            }
            soundNames.emplace_back(object.name.GetString());
        }

        return soundNames;
    }

    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames
    {
        auto loadOutcome = LoadSoundBankToBuffer(soundBankFilePath);
        return GetSoundNamesFromSoundBankFile(loadOutcome.TakeValue());
    }

    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames
    {
        return GetSoundNamesFromSoundBankFile(
            AZ::IO::Path{ DefaultBanksPath } / soundBankName.GetStringView());
    }

} // namespace BopAudio
