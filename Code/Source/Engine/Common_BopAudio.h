#pragma once

namespace BopAudio::XmlTags
{
    // Xml Element Names
    static constexpr auto EventTag = "BopAudioEvent";
    static constexpr char const* RtpcTag = "BopAudioRtpc";
    static constexpr char const* SwitchTag = "BopAudioSwitch";
    static constexpr char const* GameStateTag = "BopAudioState";
    static constexpr char const* RtpcSwitchTag = "BopAudioRtpc";
    static constexpr auto NameAttribute = "bopaudio_name";
    static constexpr auto LocalizedAttribute = "bopaudio_localized";
    static constexpr auto MultiplierAttribute = "bopaudio_multiplier";
    static constexpr auto ShiftAttribute = "atl_mult";
    static constexpr auto ValueAttribute = "atl_value";

} // namespace BopAudio::XmlTags
