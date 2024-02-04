#pragma once

#include "AzCore/Name/Name.h"

namespace BopAudio
{
    class ResourceId
    {
    public:
        ResourceId() = default;

        explicit ResourceId(AZStd::string_view resourceName)
            : m_hash(AZ::Name{ resourceName }.GetHash())
        {
        }

        explicit ResourceId(AZ::Name const& resourceName)
            : m_hash(resourceName.GetHash())
        {
        }

        constexpr auto operator==(ResourceId const& other) const -> bool
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

    /* clang-format off */
    struct AudioObjectTag{};
    struct AudioTriggerTag{};
    struct AudioEventTag{};
    struct AudioTaskTag{};
    struct InstanceTag{};
    /* clang-format on */

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

    private:
        size_t m_value;
    };

    using UniqueId = TaggedId<>;
    using AudioObjectId = TaggedId<AudioObjectTag>;
    // Represents a unique instantiation of an object.
    using InstanceId = TaggedId<InstanceTag>;

    static_assert(AZStd::is_pod_v<UniqueId>, "Must be POD.");

} // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::ResourceId>
    {
        inline auto operator()(BopAudio::ResourceId const& resourceId) const -> size_t
        {
            return resourceId.GetHash();
        }
    };
} // namespace AZStd
