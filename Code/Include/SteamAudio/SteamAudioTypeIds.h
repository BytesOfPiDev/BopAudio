
#pragma once

namespace SteamAudio
{
    // System Component TypeIds
    inline constexpr const char* SteamAudioSystemComponentTypeId = "{7152D8F8-3BFD-443D-B9D0-16FEC714FFB3}";
    inline constexpr const char* SteamAudioEditorSystemComponentTypeId = "{2018C693-1609-40FF-856C-DA319C3C1F20}";

    // Module derived classes TypeIds
    inline constexpr const char* SteamAudioModuleInterfaceTypeId = "{A269ED7D-FC56-42DC-B9C5-4ACBD84B1D4C}";
    inline constexpr const char* SteamAudioModuleTypeId = "{F8890E13-2248-4874-9EB2-7D3FEAD8D772}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* SteamAudioEditorModuleTypeId = SteamAudioModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* SteamAudioRequestsTypeId = "{88AF1717-AB02-49F8-94BE-1BB605E3DD06}";

    inline constexpr char const* SteamAudioComponentTypeId = "{A9472815-877D-4280-A54E-CBC73C72E48F}";
    inline constexpr char const* EditorSteamAudioComponentTypeId = SteamAudioComponentTypeId;

    inline constexpr auto const* AudioAssetTypeId = "{5C848E46-5F84-466F-BB74-4EFB9917422D}";
} // namespace SteamAudio
