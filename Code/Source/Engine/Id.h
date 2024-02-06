#pragma once

#include "AzCore/Name/Name.h"

namespace BopAudio
{
    /* clang-format off */
    struct AudioObjectTag{};
    struct AudioTriggerTag{};
    struct AudioEventTag{};
    struct AudioTaskTag{};
    struct InstanceTag{};
    struct UserEventTag{};
    /* clang-format on */

    template<typename Tag = void>
    class TaggedResource
    {
    public:
        TaggedResource() = default;

        explicit TaggedResource(AZStd::string_view resourceName)
            : m_hash(AZ::Name{ resourceName }.GetHash())
        {
        }

        explicit TaggedResource(AZ::Name const& resourceName)
            : m_hash(resourceName.GetHash())
        {
        }

        constexpr auto operator==(TaggedResource const& other) const -> bool
        {
            return m_hash == other.m_hash;
        }

        constexpr auto operator==(AZ::Name const& resourceName) const -> bool
        {
            return m_hash == resourceName.GetHash();
        };

        [[nodiscard]] auto ToName() const -> AZ::Name
        {
            return AZ::Name{ m_hash };
        }

        [[nodiscard]] auto ToString() const -> AZStd::string
        {
            return ToName().GetStringView();
        }

        [[nodiscard]] constexpr auto GetHash() const -> AZ::u32
        {
            return m_hash;
        }

    private:
        AZ::u32 m_hash;
    };

    using NamedResource = TaggedResource<>;

    template<typename Tag = void>
    class TaggedId
    {
    public:
        AZ_DEFAULT_COPY_MOVE(TaggedId);

        constexpr TaggedId() = default;
        constexpr TaggedId(AZ::Uuid const& guid)
            : m_value{ guid.GetHash() } {};
        ~TaggedId() = default;

        constexpr auto operator==(TaggedId const& other) -> bool
        {
            return m_value == other.m_value;
        }

        constexpr auto operator!=(TaggedId const& other) -> bool
        {
            return !(m_value == other.m_value);
        }

        [[nodiscard]] auto GetHash() const
        {
            return m_value;
        }

        [[nodiscard]] constexpr auto IsValid() const
        {
            return m_value != 0;
        }

    private:
        size_t m_value;
    };

    using UniqueId = TaggedId<>;
    using AudioObjectId = TaggedId<AudioObjectTag>;
    using AudioEventId = TaggedId<AudioEventTag>;
    // Represents a unique instantiation of an object.
    using InstanceId = TaggedId<InstanceTag>;

    static_assert(AZStd::is_pod_v<UniqueId>, "Must be POD.");

} // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::NamedResource>
    {
        inline auto operator()(BopAudio::NamedResource const& resourceId) const -> size_t
        {
            return resourceId.GetHash();
        }
    };
} // namespace AZStd
