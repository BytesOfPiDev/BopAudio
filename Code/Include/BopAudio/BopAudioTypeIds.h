
#pragma once

namespace BopAudio
{
    // System Component TypeIds
    inline constexpr char const* BopAudioSystemComponentTypeId =
        "{7152D8F8-3BFD-443D-B9D0-16FEC714FFB3}";
    inline constexpr char const* BopAudioEditorSystemComponentTypeId =
        "{2018C693-1609-40FF-856C-DA319C3C1F20}";

    // Module derived classes TypeIds
    inline constexpr char const* BopAudioModuleInterfaceTypeId =
        "{A269ED7D-FC56-42DC-B9C5-4ACBD84B1D4C}";
    inline constexpr char const* BopAudioModuleTypeId = "{F8890E13-2248-4874-9EB2-7D3FEAD8D772}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr char const* BopAudioEditorModuleTypeId = BopAudioModuleTypeId;

    // Interface TypeIds
    inline constexpr char const* BopAudioRequestsTypeId = "{88AF1717-AB02-49F8-94BE-1BB605E3DD06}";

    inline constexpr char const* BopAudioComponentTypeId = "{A9472815-877D-4280-A54E-CBC73C72E48F}";
    inline constexpr char const* EditorBopAudioComponentTypeId = BopAudioComponentTypeId;

    inline constexpr auto const* AudioAssetTypeId = "{3F61B639-9B06-4C89-A8B7-9113EFFE2C07}";
} // namespace BopAudio
