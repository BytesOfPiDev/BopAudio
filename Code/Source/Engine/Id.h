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
            : m_name{ resourceName }
        {
        }

        explicit TaggedResource(AZ::Name const& resourceName)
            : m_name{ AZStd::move(resourceName) }
        {
        }

        constexpr auto operator==(TaggedResource const& other) const -> bool
        {
            return m_name == other.m_name;
        }

        constexpr auto operator==(AZ::Name const& resourceName) const -> bool
        {
            return m_name == resourceName;
        };

        [[nodiscard]] auto ToName() const -> AZ::Name
        {
            return m_name;
        }

        [[nodiscard]] auto ToString() const -> AZStd::string
        {
            return AZStd::string{ ToName().GetStringView() };
        }

        [[nodiscard]] constexpr auto GetHash() const -> AZ::u32
        {
            return m_name.GetHash();
        }

    private:
        AZ::Name m_name{};
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
