#include "Engine/SoundBank.h"

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/JSON/rapidjson.h>

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/smart_ptr/make_shared.h"
#include "Engine/DocumentReader.h"
#include "MiniAudio/SoundAsset.h"

#include "Clients/StringUtil.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"

namespace BopAudio
{
    SoundBank::SoundBank(AZStd::string_view soundBankFileName)
        : m_id{ NamedResource(soundBankFileName) }
        , m_soundBankName{ soundBankFileName }
    {
    }

    SoundBank::SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData)
        : m_id{ NamedResource(fileEntryData ? fileEntryData->sFileName : nullptr) }
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
        auto soundNames{ GetSoundNamesFromSoundBankFile(m_bufferRef) };

        bool const isSuccess = AZStd::ranges::all_of(
            soundNames,
            [this](auto const& soundName) -> bool
            {
                SoundSource soundSource{ AZ::IO::Path{ ToCStr(soundName) } };
                soundSource.Load();

                if (!soundSource.IsReady())
                {
                    AZ_Error(
                        "SoundBank", false, "Failed to find sound asset '%s'.", ToCStr(soundName));
                    return false;
                }

                m_soundAssets[NamedResource{ soundName }] = soundSource;

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
                    AZ::IO::Path{ GetBanksRootPath() } / ToCStr(m_soundBankName)) };

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
        LoadEvents();

        AZ_Info("SoundBank", "Loaded '%s'.", ToCStr(m_soundBankName));

        return AZ::Success();
    }

    auto SoundBank::LoadSoundNames() -> bool
    {
        m_soundNames = GetSoundNamesFromSoundBankFile(m_bufferRef);

        return true;
    }

    auto SoundBank::GetSoundAsset(NamedResource const& soundName) const -> MiniAudio::SoundDataAsset
    {
        auto iter{ m_soundAssets.find(soundName) };
        if (iter == AZStd::end(m_soundAssets))
        {
            return {};
        }

        auto const& [resourceName, audioSource]{ *iter };
        return audioSource.GetAsset();
    };

    auto SoundBank::LoadEvents() -> bool
    {
        if (m_doc->HasParseError())
        {
            return false;
        }
        rapidjson::Document::ValueType const* eventsObjectPtr{ rapidjson::GetValueByPointer(
            *m_doc.get(), JsonKeys::EventsObjectKey) };

        if (!eventsObjectPtr->IsObject())
        {
            AZ_Error(
                "SoundBank",
                false,
                "Expected an object from key '%s', but got something else.",
                JsonKeys::EventsObjectKey);
            return false;
        }

        rapidjson::Document::ConstObject eventsObject{ eventsObjectPtr->GetObject() };

        if (eventsObject.ObjectEmpty())
        {
            AZ_Error(
                "SoundBank", false, "The object at key '%s' is empty.", JsonKeys::EventsObjectKey);
            return false;
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

                break;
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

            auto const basePath{ AZ::IO::Path{ JsonKeys::EventsObjectKey } /
                                 eventsMember.name.GetString() };
            /*
                        auto& newEvent{ m_events.emplace_back(m_doc, basePath) };
                        if (auto const loadOutcome{ newEvent.Load() }; !loadOutcome.IsSuccess())
                        {
                            AZ_Error(
                                "SoundBank",
                                false,
                                "Failed to process event '%s'. Reason: '%s'.",
                                ToCStr(basePath),
                                ToCStr(loadOutcome.GetError()));
                        }
                         */
        }

        return true;
    }

    auto SoundBank::CloneEvent(NamedResource resourceId) -> AZ::Outcome<AudioEvent, char const*>
    {
        auto const foundEventIter = AZStd::ranges::find_if(
            m_events,
            [&resourceId](AudioEvent const& event)
            {
                return event.GetResourceId() == resourceId;
            });

        if (foundEventIter != AZStd::end(m_events))
        {
            return AZ::Success(*foundEventIter);
        }

        return AZ::Failure("Failed to find an event with the given resource id.");
    }

    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath)
        -> AZ::Outcome<AZStd::vector<char>, char const*>
    {
        using ReturnType = decltype(LoadSoundBankToBuffer(soundBankFilePath));

        return [&soundBankFilePath]() -> ReturnType
        {
            AZ::IO::FileIOStream fileStream{ ToCStr(soundBankFilePath),
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

        if (auto outcome = ValidateDocument(doc); !outcome.IsSuccess())
        {
            AZ_Error(
                "SoundBank",
                false,
                "SoundBank failed validation. Reason: %s",
                ToCStr(outcome.TakeError()));

            return {};
        }

        auto* const soundsObjectJsonPtr{ rapidjson::GetValueByPointer(
            doc, JsonKeys::SoundFileNames) };

        if (!soundsObjectJsonPtr)
        {
            AZLOG_WARN("The SoundBank buffer is empty.");
            return {};
        }

        SoundNames soundNames{};

        for (auto& object : soundsObjectJsonPtr->GetObject())
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
