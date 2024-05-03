#pragma once

#include "ATLEntityData.h"
#include "AzCore/Name/Name.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    struct IATLAudioObjectData_bopaudio : public Audio::IATLAudioObjectData
    {
        AZ_DEFAULT_COPY_MOVE(IATLAudioObjectData_bopaudio);

        IATLAudioObjectData_bopaudio() = default;
        virtual ~IATLAudioObjectData_bopaudio() = default;

        virtual void ChangeName(AZStd::string_view objectName) = 0;

        [[nodiscard]] virtual auto GetAtlAudioObjectId() const -> Audio::TAudioObjectID = 0;
        [[nodiscard]] virtual auto GetName() const -> AZ::Name = 0;
    };

    struct IATLTriggerImplData_bopaudio : public Audio::IATLTriggerImplData
    {
        IATLTriggerImplData_bopaudio() = default;

        [[nodiscard]] virtual auto GetEventId() const -> Audio::TAudioEventID = 0;
    };

    struct IATLListenerData_script : public Audio::IATLListenerData
    {
    };

    class IATLEventData_bopaudio : public Audio::IATLEventData
    {
    public:
        [[nodiscard]] virtual auto GetAtlEventId() const -> Audio::TAudioEventID = 0;
    };

    struct IATLAudioFileEntryData_script : public Audio::IATLAudioFileEntryData
    {
    };

} // namespace BopAudio
