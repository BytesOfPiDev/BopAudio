#include "Builder/AudioEventAssetBuilderWorker.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/Debug/Trace.h"
#include "AzCore/EBus/EBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/IO/OpenMode.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "AzCore/Utils/Utils.h"
#include "AzCore/std/iterator/move_iterator.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "IAudioSystem.h"

#include "BopAudio/Util.h"
#include "Clients/AudioEventAsset.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/Tasks/Common.h"
#include "Engine/Tasks/TaskBus.h"

namespace BopAudio
{
    namespace Internal
    {
        static constexpr auto BuildAudioEventJobKey = "Build Audio Event";

        auto BuildTaskMember(rapidjson::Document::Member const& taskMember) -> AZStd::any
        {
            AZStd::string const outputBuffer = [&taskMember]() -> decltype(outputBuffer)
            {
                rapidjson::StringBuffer tempBuffer{};
                auto writer{ rapidjson::Writer(tempBuffer) };

                taskMember.value.Accept(writer);

                return { tempBuffer.GetString(), tempBuffer.GetSize() };
            }();

            AZ_Info(
                AssetBuilderSDK::InfoWindow,
                "\tBuilding Task with buffer: \n%s",
                outputBuffer.data());

            AZStd::string const taskType{ taskMember.name.GetString() };

            AZ_Info(AssetBuilderSDK::InfoWindow, "\tTask Type: %s", taskType.c_str());

            AZStd::any task{};
            TaskFactoryBus::EventResult(
                task, AZ::Crc32(taskType), &TaskFactoryRequests::Create, outputBuffer);

            AZ_Info(
                AssetBuilderSDK::InfoWindow,
                "\tTask Handlers: %zu.",
                TaskFactoryBus::GetTotalNumOfEventHandlers());

            if (task.empty())
            {
                AZ_Info(AssetBuilderSDK::InfoWindow, "\tTask not created.");
            }
            else
            {
                AZ_Info(AssetBuilderSDK::InfoWindow, "\tTask created.");
            }

            return task;
        }

        auto BuildTaskGroup(rapidjson::Document::ConstObject const& taskGroup) -> TaskContainer
        {
            AZ_Info(AssetBuilderSDK::InfoWindow, "Building Task Group");

            auto tasks = TaskContainer{};

            AZStd::ranges::for_each(
                taskGroup,
                [&tasks](auto const& taskMember)
                {
                    AZ_Info(AssetBuilderSDK::InfoWindow, "\tBuilding a task member.");
                    AZStd::any builtTask{ BuildTaskMember(taskMember) };

                    if (!builtTask.is<Task>())
                    {
                        AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Error creating Task");
                        return;
                    }

                    tasks.push_back(AZStd::any_cast<Task>(builtTask));
                });

            return AZStd::move(tasks);
        }

    } // namespace Internal

    void AudioEventAssetBuilderWorker::CreateJobs(
        AssetBuilderSDK::CreateJobsRequest const& request,
        AssetBuilderSDK::CreateJobsResponse& response) const
    {
        if (m_isShuttingDown)
        {
            response.m_result = AssetBuilderSDK::CreateJobsResultCode::ShuttingDown;
            return;
        }

        AZStd::ranges::for_each(
            request.m_enabledPlatforms,
            [&response](auto const& info)
            {
                if (info.m_identifier == "server")
                {
                    return;
                }

                AssetBuilderSDK::JobDescriptor descriptor;
                descriptor.m_jobKey = Internal::BuildAudioEventJobKey;
                descriptor.m_critical = true;
                descriptor.SetPlatformIdentifier(info.m_identifier.c_str());
                descriptor.m_priority = 0;
                response.m_createJobOutputs.push_back(descriptor);
            });

        response.m_result = AssetBuilderSDK::CreateJobsResultCode::Success;
    }

    void AudioEventAssetBuilderWorker::ProcessJob(
        AssetBuilderSDK::ProcessJobRequest const& request,
        AssetBuilderSDK::ProcessJobResponse& response) const
    {
        if (m_isShuttingDown)
        {
            AZ_Trace(
                AssetBuilderSDK::WarningWindow,
                "Cancelling job %s due to shutdown request.",
                request.m_fullPath.c_str());

            return;
        }

        if (!AZ::StringFunc::Equal(
                request.m_jobDescription.m_jobKey, Internal::BuildAudioEventJobKey))
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Trace(AssetBuilderSDK::ErrorWindow, "Job failed. Unsupported job key: '%s'") return;

            return;
        }

        AssetBuilderSDK::JobCancelListener jobCancelListener(request.m_jobId);

        if (jobCancelListener.IsCancelled())
        {
            AZ_Trace(
                AssetBuilderSDK::WarningWindow,
                "Cancel was requested for job %s.\n",
                request.m_fullPath.c_str());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Cancelled;
            return;
        }

        AZ::IO::Path const absSourcePath = [&request]() -> decltype(absSourcePath)
        {
            auto path = decltype(absSourcePath){ AZ::Utils::GetProjectPath() };
            path /= request.m_sourceFile;

            return path;
        }();

        AZStd::vector<char> const eventSourceBuffer =
            [&absSourcePath]() -> decltype(eventSourceBuffer)
        {
            AZ::IO::LocalFileIO fs{};
            AZ::IO::HandleType taskFileHandle{};

            auto const openResult{ fs.Open(
                absSourcePath.c_str(), AZ::IO::OpenMode::ModeRead, taskFileHandle) };

            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                openResult == AZ::IO::ResultCode::Success,
                "Failed to open file '%s'",
                absSourcePath.c_str());

            AZ::u64 const bufferSize = [&]() -> decltype(bufferSize)
            {
                auto size{ decltype(bufferSize){} };
                fs.Size(taskFileHandle, size);

                return size;
            }();

            AZStd::vector<char> loadedBuffer = [&bufferSize, &fs, &taskFileHandle]()
            {
                auto result{ decltype(loadedBuffer)(bufferSize) };

                fs.Read(taskFileHandle, result.data(), bufferSize);
                fs.Close(taskFileHandle);

                return result;
            }();

            return loadedBuffer;
        }();

        if (eventSourceBuffer.empty())
        {
            AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Failed to load file buffer.");
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        rapidjson::Document doc{};
        doc.Parse(eventSourceBuffer.data(), eventSourceBuffer.size());
        if (doc.HasParseError())
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Failed to parse document. Buffer: \n\n%s",
                eventSourceBuffer.data());

            return;
        }

        if (!doc.IsObject())
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Document root must be an object.");
            return;
        }

        rapidjson::Pointer tasksKeyPtr(JsonKeys::EventDocTasksKey_A.Native().data());

        auto const tasksKeyVal{ tasksKeyPtr.Get(doc) };

        if (!tasksKeyVal)
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Key '%s' does not exist.",
                JsonKeys::EventDocTasksKey_A);

            return;
        }

        if (!tasksKeyVal->IsArray())
        {
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Unexpected type at key '%s'. Expected an array.",
                JsonKeys::EventDocTasksKey_A.Native().data());
            return;
        }

        auto tasksArray{ tasksKeyVal->GetArray() };

        AZStd::vector<AudioOutcome<rapidjson::Document::ConstObject>> taskGroups{};

        // Get the task groups
        AZStd::ranges::transform(
            tasksArray.begin(),
            tasksArray.end(),
            AZStd::back_inserter(taskGroups),
            [](rapidjson::Document::GenericValue const& element) -> decltype(taskGroups)::value_type
            {
                rapidjson::Document::ValueType val{};

                if (!element.IsObject())
                {
                    return AZ::Failure("Value is not an object.");
                }
                // This should be safe if the file passed validation.
                return AZ::Success(element.GetObject());
            });

        AZ_Info(AssetBuilderSDK::InfoWindow, "Finished getting task groups");

        // Build the task groups
        TaskContainer tasks{};
        AZStd::ranges::for_each(
            taskGroups,
            [&tasks](AudioOutcome<rapidjson::Document::ConstObject> const& taskGroup)
            {
                if (!taskGroup.IsSuccess())
                {
                    AZ_Error(AssetBuilderSDK::ErrorWindow, false, "Invalid Task Group. Skipping.");
                    return;
                }
                auto builtTasks = Internal::BuildTaskGroup(taskGroup.GetValue());

                tasks.insert(
                    tasks.end(),
                    AZStd::move_iterator(builtTasks.begin()),
                    AZStd::move_iterator(builtTasks.end()));
            });

        AZ::IO::Path const absProductPath = [absSourcePath, &request]() -> decltype(absProductPath)
        {
            AZ::IO::Path path{ request.m_tempDirPath };
            path /= request.m_sourceFile;
            path.ReplaceExtension(AudioEventAsset::ProductExtensionPattern);
            return path;
        }();

        auto const newId = absProductPath.Filename().Stem().String();

        AudioEventAsset event{};
        event.m_id = Audio::AudioStringToID<Audio::TAudioControlID>(newId.c_str());

        AZ_Info(AssetBuilderSDK::InfoWindow, "AudioEvent: %s | Id: %zu", newId.c_str(), event.m_id);

        event.m_tasks = tasks;

        bool const successfullySaved = AZ::Utils::SaveObjectToFile<AudioEventAsset>(
            absProductPath.c_str(), AZ::DataStream::ST_JSON, &event);

        if (!successfullySaved)
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Failed to save product asset to builder product path '%s'",
                absProductPath.Native().data());

            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        AssetBuilderSDK::JobProduct jobProduct{ absProductPath.c_str() };
        jobProduct.m_productAssetType = AZ::AzTypeInfo<AudioEventAsset>::Uuid();
        jobProduct.m_productSubID = AudioEventAsset::AssetSubId;
        jobProduct.m_dependenciesHandled = true;

        response.m_outputProducts.push_back(jobProduct);

        response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Success;
    }
} // namespace BopAudio
