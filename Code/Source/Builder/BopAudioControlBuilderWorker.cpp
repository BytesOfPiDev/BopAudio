#include "Builder/BopAudioControlBuilderWorker.h"

#include "ATLCommon.h"
#include "AssetBuilderSDK/AssetBuilderSDK.h"
#include "AzCore/IO/SystemFile.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "AzFramework/IO/LocalFileIO.h"

#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{
    AZ_RTTI_NO_TYPE_INFO_IMPL(BopAudioControlBuilderWorker);
    AZ_TYPE_INFO_WITH_NAME_IMPL(
        BopAudioControlBuilderWorker,
        "BopAudioBuilserWorker",
        "{06B4A9AB-D205-4DAF-B36D-EEED181509B2}");
    AZ_CLASS_ALLOCATOR_IMPL(BopAudioControlBuilderWorker, AZ::SystemAllocator);

    namespace Internal
    {
        constexpr auto JsonEventsKey{ "includedEvents" };
        constexpr auto NodeDoesNotExistMessage =
            "%s node does not exist. Please be sure that you have defined at "
            "least one %s for this Audio Control file.\n";
        constexpr auto MalformedNodeMissingAttributeMessage{
            "%s node is malformed: does not have an attribute %s defined. This "
            "is likely the "
            "result of manual editing. Please resave the Audio Control file.\n"
        };
        [[maybe_unused]] constexpr auto MalformedNodeMissingChildNodeMessage{
            "%s node does not contain a child %s node. This is likely the "
            "result of manual editing. Please resave the Audio Control file.\n"
        };

        using AtlConfigGroupMap =
            AZStd::unordered_map<AZStd::string, const AZ::rapidxml::xml_node<char>*>;

        namespace Legacy
        {
            auto GetAtlPlatformName(AZStd::string const& requestPlatform) -> AZStd::string
            {
                AZStd::string atlPlatform;
                AZStd::string platform = requestPlatform;

                // When debugging a builder using a debug task, it replaces
                // platform tags with "debug platform". Make sure the builder
                //  actually uses the host platform identifier when going
                //  through this function in this case.
                if (platform == "debug platform")
                {
                    atlPlatform = AZ_TRAIT_OS_PLATFORM_NAME;
                    AZStd::to_lower(atlPlatform.begin(), atlPlatform.end());
                }
                else
                {
                    if (platform == "pc")
                    {
                        atlPlatform = "windows";
                    }
                    else if (platform == "android")
                    {
                        atlPlatform = "android";
                    }
                    else if (platform == "mac")
                    {
                        atlPlatform = "mac";
                    }
                    else
                    {
                        atlPlatform = AZStd::move(platform);
                    }
                }
                return AZStd::move(atlPlatform);
            }

            auto BuildConfigGroupMap(
                const AZ::rapidxml::xml_node<char>* preloadRequestNode,
                AtlConfigGroupMap& configGroupMap) -> AZ::Outcome<void, AZStd::string>
            {
                AZ_Assert(
                    preloadRequestNode != nullptr,
                    NodeDoesNotExistMessage,
                    Audio::ATLXmlTags::ATLPreloadRequestTag,
                    "preload request");

                auto configGroupNode =
                    preloadRequestNode->first_node(Audio::ATLXmlTags::ATLConfigGroupTag);
                while (configGroupNode)
                {
                    // Populate the config group map by iterating over all
                    // ATLConfigGroup nodes, and place each one in the map keyed
                    // by the group's atl_name attribute...
                    if (auto const configGroupNameAttr =
                            configGroupNode->first_attribute(Audio::ATLXmlTags::ATLNameAttribute))
                    {
                        configGroupMap.emplace(configGroupNameAttr->value(), configGroupNode);
                    }
                    else
                    {
                        return AZ::Failure(AZStd::string::format(
                            MalformedNodeMissingAttributeMessage,
                            Audio::ATLXmlTags::ATLConfigGroupTag,
                            Audio::ATLXmlTags::ATLNameAttribute));
                    }

                    configGroupNode =
                        configGroupNode->next_sibling(Audio::ATLXmlTags::ATLConfigGroupTag);
                }

                // If no config groups are defined, this is an empty preload
                // request with no banks referenced, which is valid.
                return AZ::Success();
            }

            auto GetBanksFromAtlPreloads(
                const AZ::rapidxml::xml_node<char>* preloadsNode,
                AZStd::string const& atlPlatformIdentifier,
                AZStd::vector<AZStd::string>& banksReferenced) -> AZ::Outcome<void, AZStd::string>
            {
                AZ_Assert(
                    preloadsNode != nullptr,
                    NodeDoesNotExistMessage,
                    Audio::ATLXmlTags::PreloadsNodeTag,
                    "preload request");

                auto preloadRequestNode =
                    preloadsNode->first_node(Audio::ATLXmlTags::ATLPreloadRequestTag);
                if (!preloadRequestNode)
                {
                    return AZ::Failure(AZStd::string::format(
                        NodeDoesNotExistMessage,
                        Audio::ATLXmlTags::ATLPreloadRequestTag,
                        "preload request"));
                }

                // For each preload request in the control file, determine which
                // config group is used for this platform and register each bank
                // listed in that preload request as a dependency.
                while (preloadRequestNode)
                {
                    AtlConfigGroupMap configGroupMap;
                    auto configGroupMapResult =
                        BuildConfigGroupMap(preloadRequestNode, configGroupMap);

                    // If the returned map is empty, there are not banks
                    // referenced in the preload request, return the result
                    // here.
                    if (!configGroupMapResult.IsSuccess() || configGroupMap.size() == 0)
                    {
                        return configGroupMapResult;
                    }

                    auto const platformsNode =
                        preloadRequestNode->first_node(Audio::ATLXmlTags::ATLPlatformsTag);
                    if (!platformsNode)
                    {
                        return AZ::Failure(AZStd::string::format(
                            MalformedNodeMissingChildNodeMessage,
                            Audio::ATLXmlTags::ATLPreloadRequestTag,
                            Audio::ATLXmlTags::ATLPlatformsTag));
                    }

                    auto platformNode =
                        platformsNode->first_node(Audio::ATLXmlTags::PlatformNodeTag);
                    if (!platformNode)
                    {
                        return AZ::Failure(AZStd::string::format(
                            MalformedNodeMissingChildNodeMessage,
                            Audio::ATLXmlTags::ATLPlatformsTag,
                            Audio::ATLXmlTags::PlatformNodeTag));
                    }

                    AZStd::string configGroupName;
                    // For each platform node in the platform list, check the
                    // atl_name to see if it matches the platform the request is
                    //  intended for. If it is, grab the name of the config
                    //  group that is used for that platform to load it.
                    while (platformNode)
                    {
                        auto const atlNameAttr =
                            platformNode->first_attribute(Audio::ATLXmlTags::ATLNameAttribute);
                        if (!atlNameAttr)
                        {
                            return AZ::Failure(AZStd::string::format(
                                MalformedNodeMissingAttributeMessage,
                                Audio::ATLXmlTags::PlatformNodeTag,
                                Audio::ATLXmlTags::ATLNameAttribute));
                        }
                        else if (atlPlatformIdentifier == atlNameAttr->value())
                        {
                            // We've found the right platform that matches the
                            // request, so grab the group name and stop looking
                            // through
                            //  the list
                            auto const configGroupNameAttr = platformNode->first_attribute(
                                Audio::ATLXmlTags::ATLConfigGroupAttribute);
                            if (!configGroupNameAttr)
                            {
                                return AZ::Failure(AZStd::string::format(
                                    MalformedNodeMissingAttributeMessage,
                                    Audio::ATLXmlTags::PlatformNodeTag,
                                    Audio::ATLXmlTags::ATLConfigGroupAttribute));
                            }
                            configGroupName = configGroupNameAttr->value();
                            break;
                        }

                        platformNode =
                            platformNode->next_sibling(Audio::ATLXmlTags::PlatformNodeTag);
                    }

                    const AZ::rapidxml::xml_node<char>* configGroupNode =
                        configGroupMap[configGroupName];
                    if (!configGroupNode)
                    {
                        // The config group this platform uses isn't defined in
                        // the control file. This might be intentional, so just
                        //  generate a warning and keep going to the next
                        //  preload node.
                        AZ_TracePrintf(
                            "Audio Control Builder",
                            "%s node for config group %s is not defined, so no "
                            "banks are referenced.",
                            Audio::ATLXmlTags::ATLConfigGroupTag,
                            configGroupName.c_str());
                    }
                    else
                    {
                        auto bopFileNode = configGroupNode->first_node(XmlTags::BopFileTag);
                        if (!bopFileNode)
                        {
                            return AZ::Failure(AZStd::string::format(
                                MalformedNodeMissingChildNodeMessage,
                                Audio::ATLXmlTags::ATLConfigGroupTag,
                                XmlTags::BopFileTag));
                        }

                        // For each BopAudioFile (soundbank) referenced in the
                        // config group, grab the file name and add it to the
                        // reference list
                        while (bopFileNode)
                        {
                            auto const bankNameAttribute =
                                bopFileNode->first_attribute(XmlTags::NameAttribute);
                            if (!bankNameAttribute)
                            {
                                return AZ::Failure(AZStd::string::format(
                                    MalformedNodeMissingAttributeMessage,
                                    XmlTags::BopFileTag,
                                    XmlTags::NameAttribute));
                            }

                            // Prepend the bank name with the relative path to
                            // the BopAudio sounds folder to get relative path
                            // to the bank from the @products@ alias and push
                            // that into the list of banks referenced.
                            AZStd::string soundsPrefix = DefaultBanksPath;
                            banksReferenced.emplace_back(soundsPrefix + bankNameAttribute->value());

                            bopFileNode = bopFileNode->next_sibling(XmlTags::BopFileTag);
                        }
                    }

                    preloadRequestNode =
                        preloadRequestNode->next_sibling(Audio::ATLXmlTags::ATLPreloadRequestTag);
                }

                return AZ::Success();
            }
        } // namespace Legacy

        auto BuildAtlEventList(
            const AZ::rapidxml::xml_node<char>* triggersNode,
            AZStd::vector<AZStd::string>& eventNames) -> AZ::Outcome<void, AZStd::string>
        {
            AZ_Assert(
                triggersNode != nullptr,
                NodeDoesNotExistMessage,
                Audio::ATLXmlTags::TriggersNodeTag,
                "trigger");

            auto triggerNode = triggersNode->first_node(Audio::ATLXmlTags::ATLTriggerTag);
            while (triggerNode)
            {
                // For each audio trigger, push the name of the BopAudio event
                // (if assigned) into the list. It's okay for an ATLTrigger node
                // to not have a BopAudio event associated with it.
                if (auto const eventNode = triggerNode->first_node(XmlTags::TriggerTag))
                {
                    if (auto const eventNameAttr =
                            eventNode->first_attribute(XmlTags::NameAttribute))
                    {
                        eventNames.push_back(eventNameAttr->value());
                    }
                    else
                    {
                        return AZ::Failure(AZStd::string::format(
                            MalformedNodeMissingAttributeMessage,
                            XmlTags::TriggerTag,
                            XmlTags::NameAttribute));
                    }
                }

                triggerNode = triggerNode->next_sibling(Audio::ATLXmlTags::ATLTriggerTag);
            }

            return AZ::Success();
        }

        auto GetBanksFromAtlPreloads(
            const AZ::rapidxml::xml_node<char>* preloadsNode,
            AZStd::vector<AZStd::string>& banksReferenced) -> AZ::Outcome<void, AZStd::string>
        {
            AZ_Assert(
                preloadsNode != nullptr,
                NodeDoesNotExistMessage,
                Audio::ATLXmlTags::PreloadsNodeTag,
                "preload request");

            auto preloadRequestNode =
                preloadsNode->first_node(Audio::ATLXmlTags::ATLPreloadRequestTag);
            if (!preloadRequestNode)
            {
                return AZ::Failure(AZStd::string::format(
                    NodeDoesNotExistMessage,
                    Audio::ATLXmlTags::ATLPreloadRequestTag,
                    "preload request"));
            }

            // Loop through the ATLPreloadRequest nodes...
            // Find any BopAudio soundbanks listed and add them to the
            // banksReferenced vector.
            while (preloadRequestNode)
            {
                // Find the first BopFileTag
                if (auto bopAudioFileNode = preloadRequestNode->first_node(XmlTags::BopFileTag))
                {
                    while (bopAudioFileNode)
                    {
                        auto const libraryNameAttr =
                            bopAudioFileNode->first_attribute(XmlTags::NameAttribute);
                        if (libraryNameAttr)
                        {
                            AZStd::string soundsPrefix = DefaultBanksPath;
                            banksReferenced.emplace_back(soundsPrefix + libraryNameAttr->value());
                        }
                        else
                        {
                            return AZ::Failure(AZStd::string::format(
                                MalformedNodeMissingAttributeMessage,
                                XmlTags::BopFileTag,
                                XmlTags::NameAttribute));
                        }

                        bopAudioFileNode = bopAudioFileNode->next_sibling(XmlTags::BopFileTag);
                    }
                }
                else
                {
                    return AZ::Failure(AZStd::string::format(
                        "Preloads Xml appears to be in an older format, trying "
                        "Legacy parsing.\n"));
                }

                preloadRequestNode =
                    preloadRequestNode->next_sibling(Audio::ATLXmlTags::ATLPreloadRequestTag);
            }

            return AZ::Success();
        }

        auto GetEventsFromBankMetadata(
            rapidjson::Value const& rootObject, AZStd::set<AZStd::string>& eventNames)
            -> AZ::Outcome<void, AZStd::string>
        {
            if (!rootObject.IsObject())
            {
                return AZ::Failure(
                    AZStd::string("The root of the metadata file is not an object. Please "
                                  "regenerate the metadata for this soundbank."));
            }

            // If the file doesn't define an events field, then there are no
            // events in the bank
            if (!rootObject.HasMember(JsonEventsKey))
            {
                return AZ::Success();
            }

            rapidjson::Value const& eventsArray = rootObject[JsonEventsKey];
            if (!eventsArray.IsArray())
            {
                return AZ::Failure(
                    AZStd::string("Events field is not an array. Please regenerate the "
                                  "metadata for this soundbank."));
            }

            for (rapidjson::SizeType eventIndex = 0; eventIndex < eventsArray.Size(); ++eventIndex)
            {
                eventNames.emplace(eventsArray[eventIndex].GetString());
            }

            return AZ::Success();
        }

        auto GetEventsFromBank(
            AZStd::string const& bankMetadataPath, AZStd::set<AZStd::string>& eventNames)
            -> AZ::Outcome<void, AZStd::string>
        {
            if (!AZ::IO::SystemFile::Exists(bankMetadataPath.c_str()))
            {
                return AZ::Failure(AZStd::string::format(
                    "Failed to find the soundbank metadata file %s. Full "
                    "dependency information cannot be determined without the "
                    "metadata "
                    "file. Please regenerate the metadata for this soundbank.",
                    bankMetadataPath.c_str()));
            }

            AZ::u64 fileSize = AZ::IO::SystemFile::Length(bankMetadataPath.c_str());
            if (fileSize == 0)
            {
                return AZ::Failure(AZStd::string::format(
                    "Soundbank metadata file at path %s is an empty file. "
                    "Please regenerate the metadata for this soundbank.",
                    bankMetadataPath.c_str()));
            }

            AZStd::vector<char> buffer(fileSize + 1);
            buffer[fileSize] = 0;
            if (!AZ::IO::SystemFile::Read(bankMetadataPath.c_str(), buffer.data()))
            {
                return AZ::Failure(AZStd::string::format(
                    "Failed to read the soundbank metadata file at path %s. "
                    "Please make sure the file is not open or being edited by "
                    "another "
                    "program.",
                    bankMetadataPath.c_str()));
            }

            rapidjson::Document bankMetadataDoc;
            bankMetadataDoc.Parse(buffer.data());
            if (bankMetadataDoc.GetParseError() != rapidjson::ParseErrorCode::kParseErrorNone)
            {
                return AZ::Failure(AZStd::string::format(
                    "Failed to parse soundbank metadata at path %s into JSON. "
                    "Please regenerate the metadata for this soundbank.",
                    bankMetadataPath.c_str()));
            }

            return GetEventsFromBankMetadata(bankMetadataDoc, eventNames);
        }

    } // namespace Internal

    void BopAudioControlBuilderWorker::CreateJobs(
        AssetBuilderSDK::CreateJobsRequest const& request,
        AssetBuilderSDK::CreateJobsResponse& response)
    {
        if (m_isShuttingDown)
        {
            response.m_result = AssetBuilderSDK::CreateJobsResultCode::ShuttingDown;
            return;
        }

        for (AssetBuilderSDK::PlatformInfo const& info : request.m_enabledPlatforms)
        {
            if (info.m_identifier == "server")
            {
                continue;
            }

            AssetBuilderSDK::JobDescriptor descriptor;
            descriptor.m_jobKey = "Audio Control";
            descriptor.m_critical = true;
            descriptor.SetPlatformIdentifier(info.m_identifier.c_str());
            descriptor.m_priority = 0;
            response.m_createJobOutputs.push_back(descriptor);
        }

        response.m_result = AssetBuilderSDK::CreateJobsResultCode::Success;
    }

    void BopAudioControlBuilderWorker::ProcessJob(
        [[maybe_unused]] AssetBuilderSDK::ProcessJobRequest const& request,
        AssetBuilderSDK::ProcessJobResponse& response)
    {
        AZ_TracePrintf(AssetBuilderSDK::InfoWindow, "AudioControlBuilderWorker Starting Job.\n");

        if (m_isShuttingDown)
        {
            AZ_TracePrintf(
                AssetBuilderSDK::WarningWindow,
                "Cancelled job %s because shutdown was requested.\n",
                request.m_fullPath.c_str());
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Cancelled;
            return;
        }

        AZStd::string fileName;
        AZ::StringFunc::Path::GetFullFileName(request.m_fullPath.c_str(), fileName);

        AssetBuilderSDK::JobProduct jobProduct(request.m_fullPath);

        if (!ParseProductDependencies(
                request, jobProduct.m_dependencies, jobProduct.m_pathDependencies))
        {
            AZ_Error(
                AssetBuilderSDK::ErrorWindow,
                false,
                "Error during parsing product dependencies for asset %s.\n",
                fileName.c_str());
            response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Failed;
            return;
        }

        response.m_outputProducts.push_back(jobProduct);
        response.m_resultCode = AssetBuilderSDK::ProcessJobResult_Success;
    }

    void BopAudioControlBuilderWorker::ShutDown()
    {
    }

    auto BopAudioControlBuilderWorker::ParseProductDependencies(
        AssetBuilderSDK::ProcessJobRequest const& request,
        AZStd::vector<AssetBuilderSDK::ProductDependency>& productDependencies,
        AssetBuilderSDK::ProductPathDependencySet& pathDependencies) -> bool
    {
        AZ::IO::FileIOStream fileStream;
        if (!fileStream.Open(request.m_fullPath.c_str(), AZ::IO::OpenMode::ModeRead))
        {
            return false;
        }

        AZ::IO::SizeType length = fileStream.GetLength();
        if (length == 0)
        {
            return false;
        }

        AZStd::vector<char> charBuffer;
        charBuffer.resize_no_construct(length + 1);
        fileStream.Read(length, charBuffer.data());
        charBuffer.back() = 0;

        // Get the XML root node
        AZ::rapidxml::xml_document<char> xmlDoc;
        if (!xmlDoc.parse<AZ::rapidxml::parse_no_data_nodes>(charBuffer.data()))
        {
            return false;
        }

        AZ::rapidxml::xml_node<char>* xmlRootNode = xmlDoc.first_node();
        if (!xmlRootNode)
        {
            return false;
        }

        ParseProductDependenciesFromXmlFile(
            xmlRootNode,
            request.m_fullPath,
            request.m_sourceFile,
            request.m_platformInfo.m_identifier,
            productDependencies,
            pathDependencies);

        return true;
    }

    void BopAudioControlBuilderWorker::ParseProductDependenciesFromXmlFile(
        const AZ::rapidxml::xml_node<char>* node,
        AZStd::string const& fullPath,
        [[maybe_unused]] AZStd::string const& sourceFile,
        [[maybe_unused]] AZStd::string const& platformIdentifier,
        [[maybe_unused]] AZStd::vector<AssetBuilderSDK::ProductDependency>& productDependencies,
        AssetBuilderSDK::ProductPathDependencySet& pathDependencies)
    {
        AZ_Assert(
            node != nullptr,
            "AudioControlBuilderWorker::ParseProductDependenciesFromXmlFile - "
            "null xml root node!\n");

        auto const preloadsNode = node->first_node(Audio::ATLXmlTags::PreloadsNodeTag);
        if (!preloadsNode)
        {
            // No preloads were defined in this control file, so we can return.
            // If triggers are defined in this preload file, we can't validate
            // that they'll be playable because we are unsure of what other
            // control files for the given scope are defined.
            return;
        }

        // Collect any references to soundbanks, initially use the newer parsing
        // format...
        AZStd::vector<AZStd::string> banksReferenced;
        AZ::Outcome<void, AZStd::string> gatherBankReferencesResult =
            Internal::GetBanksFromAtlPreloads(preloadsNode, banksReferenced);

        if (!gatherBankReferencesResult)
        {
            AZ_Warning(
                "Audio Control Builder",
                false,
                "Failed to gather product dependencies for Audio Control file "
                "%s.  %s\n",
                sourceFile.c_str(),
                gatherBankReferencesResult.GetError().c_str());
            return;
        }

        if (banksReferenced.size() == 0)
        {
            // If there are no banks referenced, then there are no dependencies
            // to register, so return.
            return;
        }

        for (AZStd::string const& relativeBankPath : banksReferenced)
        {
            pathDependencies.emplace(
                relativeBankPath, AssetBuilderSDK::ProductPathDependencyType::ProductFile);
        }

        // For each bank figure out what events are included in the bank, then
        // run through every event referenced in the file and
        //  make sure it is in the list gathered from the banks.
        auto const triggersNode = node->first_node(Audio::ATLXmlTags::TriggersNodeTag);
        if (!triggersNode)
        {
            // No triggers were defined in this file, so we don't need to do any
            // event validation.
            return;
        }

        AZStd::vector<AZStd::string> eventsReferenced;
        AZ::Outcome<void, AZStd::string> gatherEventReferencesResult =
            Internal::BuildAtlEventList(triggersNode, eventsReferenced);
        if (!gatherEventReferencesResult.IsSuccess())
        {
            AZ_Warning(
                "Audio Control Builder",
                false,
                "Failed to gather list of events referenced by Audio Control "
                "file %s. %s",
                sourceFile.c_str(),
                gatherEventReferencesResult.GetError().c_str());
            return;
        }

        AZStd::string projectSourcePath = fullPath;
        AZ::u64 firstSubDirectoryIndex =
            AZ::StringFunc::Find(projectSourcePath, m_globalScopeControlsPath);
        AZ::StringFunc::LKeep(projectSourcePath, firstSubDirectoryIndex);

        AZStd::set<AZStd::string> bopAudioTriggersInReferencedBanks;

        // Load all bankdeps files for all banks referenced and aggregate the
        // list of events in those files.
        for (AZStd::string const& relativeBankPath : banksReferenced)
        {
            // Create the full path to the bankdeps file from the bank file.
            AZStd::string bankMetadataPath;
            AZ::StringFunc::Path::Join(
                projectSourcePath.c_str(), relativeBankPath.c_str(), bankMetadataPath);
            AZ::StringFunc::Path::ReplaceExtension(
                bankMetadataPath, SoundbankDependencyFileExtension);

            AZ::Outcome<void, AZStd::string> getReferencedEventsResult =
                Internal::GetEventsFromBank(bankMetadataPath, bopAudioTriggersInReferencedBanks);
            if (!getReferencedEventsResult.IsSuccess())
            {
                // only warn if we couldn't get info from a bankdeps file. Won't
                // impact registering dependencies, but used to help customers
                // potentially debug issues.
                AZ_Warning(
                    "Audio Control Builder",
                    false,
                    "Failed to gather list of events referenced by soundbank "
                    "%s. %s",
                    relativeBankPath.c_str(),
                    getReferencedEventsResult.GetError().c_str());
            }
        }

        // Confirm that each event referenced by the file exists in the list of
        // events available from the banks referenced.
        for (AZStd::string const& eventInControlFile : eventsReferenced)
        {
            if (bopAudioTriggersInReferencedBanks.find(eventInControlFile) ==
                bopAudioTriggersInReferencedBanks.end())
            {
                AZ_Warning(
                    "Audio Control Builder",
                    false,
                    "Failed to find BopAudio event %s in the list of events "
                    "contained in banks referenced by %s. Event may fail to "
                    "play "
                    "properly.",
                    eventInControlFile.c_str(),
                    sourceFile.c_str());
            }
        }
    }

} // namespace BopAudio
