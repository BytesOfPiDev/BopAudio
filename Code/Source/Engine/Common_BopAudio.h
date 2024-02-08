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
    static constexpr auto SoundDataTag = "bopaudio_soundData";
    static constexpr auto MultiplierAttribute = "atl_mult";
    static constexpr auto ShiftAttribute = "atl_shift";
    static constexpr auto LocalizedAttribute = "bopaudio_localized";

} // namespace BopAudio::XmlTags

namespace BopAudioStrings
{
    // Project Folders
    static constexpr auto GameParametersFolder = "GameParameters";
    static constexpr auto EventsFolder = "Events";

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

} // namespace BopAudioStrings

namespace BopAudio::JsonKeys
{
    static constexpr char BopAudioFile[] = "/BopAudioDocument";
    static constexpr char SoundFileNames[] = "/BopAudioDocument/Sounds";
    static constexpr char EventsObjectKey[]{ "/BopAudioDocument/Events" };

    static constexpr char PlayEventKey[]{ "/Play" };
    static constexpr char PlayEventResourceKey[]{ "/Play/Resource" };
    static constexpr char StopEventKey[]{ "/Stop" };
    static constexpr char StopEventResourceKey[]{ "/Stop/Resource" };
} // namespace BopAudio::JsonKeys

namespace BopAudio
{
    static constexpr char DocumentJsonSchema[] =
        R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "object",
        "properties": {
          "BopAudioDocument": {
            "type": "object",
            "properties": {
              "Sounds": {
                "type": "object",
                "additionalProperties": {
                  "type": "object"
                }
              },
              "Events": {
                "type": "object",
                "additionalProperties": {
                  "type": "object",
                  "properties": {
                    "Tasks": {
                      "type": "array",
                      "items": {
                        "type": "object",
                        "properties": {
                          "Play": {
                            "type": "object",
                            "properties": {
                              "Resource": {
                                "type": "string"
                              }
                            },
                            "required": [
                              "Resource"
                            ],
                            "additionalProperties": false
                          },
                          "Stop": {
                            "type": "object",
                            "properties": {
                              "Resource": {
                                "type": "string"
                              }
                            },
                            "required": [
                              "Resource"
                            ],
                            "additionalProperties": false
                          }
                        },
                        "additionalProperties": false
                      }
                    }
                  },
                  "required": [
                    "Tasks"
                  ],
                  "additionalProperties": false
                }
              }
            },
            "additionalProperties": false
          }
        },
        "required": [
          "BopAudioDocument"
        ],
        "additionalProperties": false
      })";
}
