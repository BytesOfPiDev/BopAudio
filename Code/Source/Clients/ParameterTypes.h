#pragma once

#include <AzCore/std/typetraits/is_pod.h>

#include "AzCore/Serialization/SerializeContext_fwd.h"
#include "IAudioInterfacesCommonData.h"

namespace BopAudio
{
    struct StartEventData
    {
        AZ_TYPE_INFO(StartEventData, "{25492250-BAB6-4D5E-85F0-34B64F1F06DF}");

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetAudioEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

        [[nodiscard]] auto GetAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_audioObjectId;
        }

        void* m_owner;
        Audio::TAudioEventID m_audioEventId;
        Audio::TAudioObjectID m_audioObjectId;
    };

    struct StopEventData
    {
        AZ_TYPE_INFO(StopEventData, "{9E2A41C2-E2DD-498B-9D46-857204B20DC0}");

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetAudioEventId() const -> Audio::TAudioEventID
        {
            return m_audioEventId;
        }

        [[nodiscard]] auto GetAudioObjectId() const -> Audio::TAudioObjectID
        {
            return m_audioObjectId;
        }

        void* m_owner;
        Audio::TAudioEventID m_audioEventId;
        Audio::TAudioObjectID m_audioObjectId;
    };

    static_assert(AZStd::is_pod_v<StartEventData>);
    static_assert(AZStd::is_pod_v<StopEventData>);
}