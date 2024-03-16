#include "Engine/SoundBankUtil.h"

#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/JSON/rapidjson.h>
#include <rapidjson/document.h>

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/OpenMode.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzFramework/IO/LocalFileIO.h"

#include "Engine/Common_BopAudio.h"
#include "Engine/DocumentReader.h"
#include "Engine/Id.h"
#include "IAudioSystem.h"

namespace BopAudio
{
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

            eventIds.emplace_back(
                Audio::AudioStringToID<AudioEventId>(eventsMember.name.GetString()));
        }

        AZ_Info("LoadEventIds", "Events found: [%i]", eventIds.size());
        return AZ::Success(eventIds);
    }

    auto LoadSoundBankToBuffer(AZ::IO::PathView soundBankFilePath)
        -> AudioOutcome<AZStd::vector<char>>
    {
        AZ::IO::LocalFileIO fileIO{};

        AZ::IO::HandleType fileHandle = AZ::IO::InvalidHandle;
        auto const openResult{ fileIO.Open(
            soundBankFilePath.Native().data(), AZ::IO::OpenMode::ModeRead, fileHandle) };

        if (openResult != AZ::IO::ResultCode::Success)
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to open file '%s'", soundBankFilePath.Native().data()));
        }

        AZ::u64 const bufferSize = [&fileIO, &fileHandle]()
        {
            AZ::u64 result{};
            fileIO.Size(fileHandle, result);
            return result;
        }();

        AZStd::vector<char> const buffer = [&]() -> decltype(buffer)
        {
            auto tempBuffer{ decltype(buffer)(bufferSize) };
            fileIO.Read(fileHandle, tempBuffer.data(), tempBuffer.size());
            return tempBuffer;
        }();

        if (buffer.empty())
        {
            return AZ::Failure(AZStd::string::format(
                "Failed to read file '%s'", soundBankFilePath.Native().data()));
        }

        AZ_Warning(
            "LoadSoundBankToBuffer",
            buffer.size() != 0,
            "The sound bank file [%s] is empty!",
            soundBankFilePath.Native().data());

        return AZ::Success(AZStd::move(buffer));
    }

    auto GetSoundNamesFromSoundBankFile(rapidjson::Document const& doc) -> SoundNames
    {
        if (auto const outcome = ValidateDocument(doc); !outcome.IsSuccess())
        {
            AZ_Error(
                "SoundBank",
                false,
                "SoundBank failed validation. Reason: [%s]",
                outcome.GetError().c_str());

            return {};
        }
        rapidjson::Pointer const soundsObjectKeyPtr{ JsonKeys::SoundsKey_O.Native().data() };
        auto* const soundsObjectValue{ rapidjson::GetValueByPointer(doc, soundsObjectKeyPtr) };

        if (!soundsObjectValue)
        {
            AZLOG_INFO("No sounds found at ['%s'].", JsonKeys::SoundsKey_O.Native().data());
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

    auto GetSoundNamesFromSoundBankFile(AZStd::span<char const> soundBankFileBuffer) -> SoundNames
    {
        // TODO: Investigate why loading the doc from a function and moving it into 'doc' results in
        // crash.
        rapidjson::Document doc{};
        doc.Parse(soundBankFileBuffer.data(), soundBankFileBuffer.size());

        return GetSoundNamesFromSoundBankFile(doc);
    }

    auto GetSoundNamesFromSoundBankFile(AZ::IO::PathView soundBankFilePath) -> SoundNames
    {
        auto loadOutcome = LoadSoundBankToBuffer(soundBankFilePath);
        return GetSoundNamesFromSoundBankFile(loadOutcome.TakeValue());
    }

} // namespace BopAudio
