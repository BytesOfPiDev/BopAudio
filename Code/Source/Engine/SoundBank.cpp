#include "Engine/SoundBank.h"

#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Console/ILogger.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Outcome/Outcome.h"
#include "Engine/DocumentReader.h"
#include "MiniAudio/SoundAsset.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

#include "Clients/StringUtil.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"
#include "Engine/Events/PlaySound.h"
#include "Engine/Id.h"
#include "Engine/SoundSource.h"
#include "Engine/Task.h"

namespace BopAudio
{
    namespace Internal
    {
        auto MakeSoundBankId(AZStd::string_view fileName) -> NamedResource
        {
            auto const soundBankPath =
                fileName.empty() ? nullptr : AZ::IO::Path{ GetBanksRootPath() } / fileName;
            return NamedResource{ soundBankPath.empty() ? "" : soundBankPath.Native() };
        };

    } // namespace Internal

    SoundBank::SoundBank(AZStd::string_view soundBankFileName)
        : m_id{ Internal::MakeSoundBankId(soundBankFileName) }
        , m_soundBankName{ soundBankFileName }
    {
    }

    SoundBank::SoundBank(Audio::SATLAudioFileEntryInfo* fileEntryData)
        : m_id{ Internal::MakeSoundBankId(fileEntryData ? fileEntryData->sFileName : nullptr) }
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

    auto SoundBank::LoadSounds(AZStd::span<char const> soundBankFileBuffer) -> bool
    {
        auto const banksRootPath{ AZ::IO::Path(GetBanksRootPath()) };

        auto soundNames{ GetSoundNamesFromSoundBankFile(soundBankFileBuffer) };

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

    auto SoundBank::Load() -> AZ::Outcome<void, char const*>
    {
        // We first try to load using the buffer that the AudioSystemInterface has.
        auto const loadedFromAsiBuffer = [this]() -> bool
        {
            AZStd::span<char> asiBuffer = m_fileEntryInfo
                ? decltype(asiBuffer){ static_cast<char*>(m_fileEntryInfo->pFileData),
                                       m_fileEntryInfo->nSize }
                : decltype(asiBuffer){};

            return LoadSounds(asiBuffer);
        }();

        // If that didn't work, we try to load the buffer using the name of the sound bank.
        // TODO: Verify:
        // This should generally only trigger for the default soundbank, since I don't think that
        // the ATL tries to load it, or, if it does, doesn't automatically pass it to the ASI
        // implementation.
        if (!loadedFromAsiBuffer)
        {
            if (auto fileBuffer = LoadSoundBankToBuffer(
                    AZ::IO::Path{ GetBanksRootPath() } / ToCStr(m_soundBankName));
                !fileBuffer.empty())
            {
                LoadSounds(fileBuffer);
            }
        }

        LoadEvents();

        return AZ::Success();
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
        auto const loadedBuffer{ LoadSoundBankToBuffer(
            AZ::IO::Path{ GetBanksRootPath() } / m_soundBankName.GetCStr()) };

        rapidjson::Document const doc{ LoadDocument(loadedBuffer) };

        return LoadEvents(doc);
    }

    auto SoundBank::LoadEvents(rapidjson::Document const& doc) -> bool
    {
        if (doc.HasParseError())
        {
            return false;
        }
        rapidjson::Document::ValueType const* eventsObjectPtr{ rapidjson::GetValueByPointer(
            doc, JsonKeys::EventsObjectKey) };

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

        static constexpr auto buildTaskGroupObject =
            [](rapidjson::Document::GenericValue const& taskGroupObject) -> AZStd::vector<TaskData>
        {
            AZStd::vector<TaskData> tasks{};

            if (auto* playResource =
                    rapidjson::GetValueByPointer(taskGroupObject, JsonKeys::PlayEventResourceKey))
            {
                if (!playResource->IsString())
                {
                    AZ_Error(
                        "SoundBank",
                        false,
                        "Expected the value of an event with key '%s' to be a string, but it was "
                        "something else.",
                        JsonKeys::PlayEventResourceKey);

                    return {};
                }

                PlaySoundData playSoundData{};
                playSoundData.m_resourceId = NamedResource{ playResource->GetString() };
                tasks.emplace_back(playSoundData);

                AZLOG(
                    AE_miniaudio,
                    "Created task: [Type: 'Play', ResourceId: '%s']",
                    playResource->GetString());
            }

            return tasks;
        };

        static constexpr auto buildEachTaskInArray =
            [](rapidjson::Document::ConstArray const& taskArray)
        {
            AZLOG(AE_miniaudio, "\tFound %u elements in the task array.\n", taskArray.Size());

            for (auto const& taskElement : taskArray)
            {
                buildTaskGroupObject(taskElement);
            }
        };

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

            buildEachTaskInArray(tasksMember->value.GetArray());
        }

        return false;
    }

    auto LoadSoundBankToBuffer(AZ::IO::Path soundBankFilePath) -> AZStd::vector<char>
    {
        using ReturnType = decltype(LoadSoundBankToBuffer(soundBankFilePath));

        return [&soundBankFilePath]() -> ReturnType
        {
            AZ::IO::FileIOStream fileStream{ ToCStr(soundBankFilePath),
                                             AZ::IO::OpenMode::ModeRead };
            ReturnType tempBuffer(fileStream.GetLength());
            fileStream.Read(tempBuffer.size(), tempBuffer.data());

            return tempBuffer;
        }();
    }

    auto GetSoundNamesFromSoundBankFile(AZStd::span<char const> soundBankFileBuffer) -> SoundNames
    {
        rapidjson::Document const doc{ LoadDocument(soundBankFileBuffer) };

        if (auto outcome = ValidateDocument(doc); !outcome.IsSuccess())
        {
            AZ_Error(
                "SoundBank", false, "SoundBank failed validation. Reason: %s", outcome.TakeError());

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
        AZStd::vector<char> buffer = LoadSoundBankToBuffer(soundBankFilePath);
        return GetSoundNamesFromSoundBankFile(buffer);
    }

    auto GetSoundNamesFromSoundBankFile(AZ::Name const& soundBankName) -> SoundNames
    {
        return GetSoundNamesFromSoundBankFile(
            AZ::IO::Path{ DefaultBanksPath } / soundBankName.GetStringView());
    }

} // namespace BopAudio
