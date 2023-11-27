#pragma once

namespace SteamAudio::XmlTags
{
    // Xml Element Names
    static constexpr auto EventTag = "SAEvent";
    static constexpr char const* RtpcTag = "SARtpc";
    static constexpr char const* SwitchTag = "SASwitch";
    static constexpr char const* GameStateTag = "SAState";
    static constexpr char const* RtpcSwitchTag = "SARtpc";
    static constexpr auto NameAttribute = "sa_name";
    static constexpr auto LocalizedAttribute = "sa_localized";
    static constexpr auto MultiplierAttribute = "sa_multiplier";
    static constexpr auto ShiftAttribute = "atl_mult";
    static constexpr auto ValueAttribute = "atl_value";

} // namespace SteamAudio::XmlTags
