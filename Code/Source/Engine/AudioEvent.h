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
    namespace Internal
    {
        struct AudioEventFactory;
    }

    class AudioObject;
    class AudioEvent
    {
        friend struct Internal::AudioEventFactory;

    public:
        AZ_DEFAULT_COPY_MOVE(AudioEvent);
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEvent);
        AZ_CLASS_ALLOCATOR_DECL;

        /*
         * Default constructor.
         *
         * TODO: Make protected after figuring out how to serialize classes with protected
         */
        AudioEvent() = default;
        ~AudioEvent() = default;

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] static auto CreateFromSource(
            rapidjson::Document const& doc, ResourceRef const& eventPath)
            -> AudioOutcome<AudioEvent>;
        [[nodiscard]] static auto CreateFromSource(
            rapidjson::Document const& doc, AZ::IO::Path const& eventPath)
            -> AudioOutcome<AudioEvent>;

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

        void operator()(AudioObject& audioObject);

    private:
        auto Execute(AudioObject& audioObject) -> AZ::Outcome<void, char const*>;

    private:
        /*
         * The reference/id/name of this event.
         *
         * NOTE: This is what you use to instantiate a new event that matches how this instance was
         * created.
         */
        AudioEventId m_id{};
        /*
         * Stores implementation data that implements the event.
         *
         * It is type-erased to avoid putting the, potentially many, template/variant types in the
         * header, allowing the implementation to be be held entirely in the source file. A PIMPL
         * pattern may be better, which can be evaluated at a later time.
         */
        AZStd::any m_internalData{};
        /*
         * The current state of the event.
         *
         * It is not serialized, as it is only meaningful at run-time.
         *
         * TODO: Consider finding another way to track state that doesn't involve storing it here.
         * Perhaps pass in a state object that is updated by this event when it executes.
         */
        Audio::EAudioEventState m_eventState{};
    };

    using AudioEvents = AZStd::vector<AudioEvent>;

} // namespace BopAudio
