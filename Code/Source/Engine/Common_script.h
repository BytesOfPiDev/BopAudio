#pragma once

#include <AzCore/IO/Path/Path.h>

namespace BopAudio::XmlTags
{
    // Xml Element Names
    static constexpr auto TriggerTag = "BopAudioTrigger";
    static constexpr auto RtpcTag = "BopAudioRtpc";
    static constexpr auto SwitchTag = "BopAudioSwitch";
    static constexpr auto GameStateTag = "BopAudioState";
    static constexpr auto RtpcSwitchTag = "BopAudioRtpc";
    static constexpr auto BopFileTag = "BopAudioFile";
    static constexpr auto AuxBusTag = "BopAudioAuxBus";
    static constexpr auto ValueTag = "BopAudioValue";
    static constexpr auto NameAttribute = "bopaudio_name";
    static constexpr auto ValueAttribute = "bopaudio_value";
    static constexpr auto SoundDataTag = "bopaudio_soundData";
    static constexpr auto MultiplierAttribute = "atl_mult";
    static constexpr auto ShiftAttribute = "atl_shift";
    static constexpr auto LocalizedAttribute = "bopaudio_localized";

} // namespace BopAudio::XmlTags

namespace AudioStrings
{
    // Project Folders
    static constexpr auto GameParametersFolder = "GameParameters";
    static constexpr auto EventsFolder = "events";

    // Xml Tags
    static constexpr auto EventTag = "Event";
    static constexpr auto NameAttribute = "Name";
    static constexpr auto SoundAttribute = "Sound";
    static constexpr auto SoundFile = "SoundFile";
    static constexpr auto ValueAttribute = "Value";
    static constexpr auto AuxBusTag = "AuxBus";
    static constexpr auto SwitchGroupTag = "SwitchGroup";
    static constexpr auto StateGroupTag = "StateGroup";
    static constexpr auto ChildrenListTag = "ChildrenList";

    // Json Tags
    static constexpr auto TasksTag{ "Tasks" };
    static constexpr auto EventPlayTag = "Play";
    static constexpr auto EventStopTag = "Stop";

} // namespace AudioStrings

namespace JsonKeys
{
    static constexpr AZ::IO::PathView SoundsKey_O = "/BopAudioDocument/Sounds";
    static constexpr AZ::IO::PathView EventsKey_O{ "/BopAudioDocument/Events" };
    static constexpr AZ::IO::PathView EventDocNameKey_S{ "/EventDocument/Name" };
    static constexpr AZ::IO::PathView EventDocTasksKey_A{ "/EventDocument/Tasks" };

    static constexpr AZ::IO::PathView PlayResourceName = "/Resource";
    static constexpr AZ::IO::PathView StopEventResourceName{ "/Resource" };
} // namespace JsonKeys
