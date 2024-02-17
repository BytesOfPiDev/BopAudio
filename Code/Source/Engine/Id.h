#pragma once

#include <AzCore/IO/Path/Path.h>

#include "AzCore/Name/Name.h"
#include "AzCore/std/containers/vector.h"

namespace BopAudio
{
    /* clang-format off */
    struct AudioObjectTag{};
    struct AudioTriggerTag{};
    struct SoundAssetTag{};
    struct AudioEventTag{};
    struct AudioTaskTag{};
    struct InstanceTag{};
    struct UserEventTag{};
    /* clang-format on */

    template<typename Tag = void>
    class TaggedResource
    {
    public:
        AZ_DEFAULT_COPY_MOVE(TaggedResource);

        TaggedResource() = default;
        ~TaggedResource() = default;

        explicit TaggedResource(AZStd::string_view resourceName)
            : m_name{ resourceName }
        {
        }

        explicit TaggedResource(AZ::Name resourceName)
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

        [[nodiscard]] constexpr auto GetCStr() const -> char const*
        {
            return m_name.GetCStr();
        }

        [[nodiscard]] constexpr auto c_str() const -> char const*
        {
            return GetCStr();
        }

        [[nodiscard]] constexpr auto GetHash() const -> AZ::u32
        {
            return m_name.GetHash();
        }

        [[nodiscard]] constexpr auto GetAsPath() const -> AZ::IO::PathView
        {
            return m_name.IsEmpty() ? AZ::IO::PathView{} : m_name.GetStringView();
        }

        [[nodiscard]] constexpr auto IsValid() const -> bool
        {
            return !m_name.IsEmpty();
        }

    private:
        AZ::Name m_name{};
    };

    /*
     * A textual reference to a resource.
     *
     * In regards to this particular type, a resource can be literally anything. It can refer to
     * something 'physical', such a sound file on disk, or something more 'abstract', such as a
     * particular instance of a sound file in memory.
     *
     * While there are no restrictions on what it can be referenced, it is intended to be reference
     * a 'resource' that can be instantiated for use.
     */
    using ResourceRef = TaggedResource<>;
    using SoundRef = TaggedResource<SoundAssetTag>;

    using AudioEventId = TaggedResource<AudioEventTag>;
    using AudioEventIdContainer = AZStd::vector<AudioEventId>;

    template<typename Tag = void>
    class TaggedId
    {
    public:
        AZ_DEFAULT_COPY_MOVE(TaggedId);

        constexpr TaggedId() = default;
        constexpr TaggedId(AZ::Uuid const& guid)
            : m_value{ guid.GetHash() } {};
        ~TaggedId() = default;

        [[nodiscard]] constexpr auto operator==(TaggedId const& other) -> bool
        {
            return m_value == other.m_value;
        }

        [[nodiscard]] constexpr auto operator!=(TaggedId const& other) -> bool
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

} // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::ResourceRef>
    {
        inline auto operator()(BopAudio::ResourceRef const& resourceId) const -> size_t
        {
            return resourceId.GetHash();
        }
    };
} // namespace AZStd
