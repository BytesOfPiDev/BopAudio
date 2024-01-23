#pragma once

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
    static constexpr auto MultiplierAttribute = "atl_mult";
    static constexpr auto ShiftAttribute = "atl_shift";
    static constexpr auto LocalizedAttribute = "bopaudio_localized";

} // namespace BopAudio::XmlTags
