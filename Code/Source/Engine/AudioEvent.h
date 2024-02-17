#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>
#include <AzCore/RTTI/TypeInfoSimple.h>

#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/any.h"
#include "BopAudio/Util.h"
#include "IAudioInterfacesCommonData.h"

#include "Engine/Id.h"

namespace BopAudio
{
    class AudioObject;
    class AudioEvent
    {
    public:
        AZ_DEFAULT_COPY_MOVE(AudioEvent);
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEvent);

        ~AudioEvent() = default;

        [[nodiscard]] static auto CreateFromSource(
            rapidjson::Document& doc, ResourceRef const& eventPath) -> AudioOutcome<AudioEvent>;
        [[nodiscard]] static auto CreateFromSource(
            rapidjson::Document& doc, AZ::IO::Path const& eventPath) -> AudioOutcome<AudioEvent>;

        [[nodiscard]] constexpr auto GetState() const -> Audio::EAudioEventState
        {
            return m_eventState;
        }

        [[nodiscard]] auto GetId() const -> AudioEventId
        {
            return m_id;
        }

        [[nodiscard]] auto IsResource(ResourceRef const& resourceId) const -> bool
        {
            return m_id.IsValid() && (m_id.ToName() == resourceId.ToName());
        }

        auto Execute(AudioObject& audioObject) -> AZ::Outcome<void, char const*>;

    protected:
        AudioEvent() = default;

    private:
        AudioEventId m_id{};
        AZStd::any m_internalData{};
        Audio::EAudioEventState m_eventState{};
    };

} // namespace BopAudio
