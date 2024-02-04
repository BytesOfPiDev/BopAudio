#pragma once

#include "AzCore/IO/Path/Path.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"

#include "AzCore/Name/Name.h"
#include "AzCore/std/containers/vector.h"
#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{
    /* clang-format off */
    struct AudioEventTag{ static constexpr AZ::IO::PathView RefPath{"sounds/bopaudio/events"}; };
    struct AudioObjectTag{};
    struct AudioTaskTag{};
    struct AudioTriggerTag{};
    struct InstanceTag{};
    struct UserEventTag{};
    struct SoundAssetTag{  };
    struct SoundBankTag{ static constexpr AZ::IO::PathView RefPath{DefaultBanksPath}; };
    /* clang-format on */

    template<typename Tag>
    class TaggedResource;

    AZ_TYPE_INFO_TEMPLATE_WITH_NAME(
        TaggedResource, , "{F10E4E0B-AABF-4927-9EE5-87CF82A398E0}", AZ_TYPE_INFO_CLASS);

    template<typename Tag = void>
    class TaggedResource
    {
        using Self = TaggedResource<Tag>;

        template<typename>
        friend void ReflectTaggedResource(AZ::ReflectContext*);

    public:
        AZ_DEFAULT_COPY_MOVE(TaggedResource);

        static void Reflect(AZ::ReflectContext* context);

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

        /*
         * Represents the reference's "filename" (not necessarily an actual file).
         *
         * e.g. "/sounds/dragons/thomas_the_tank" -> "thomas_the_tank"
         * e.g. "/sounds/dragons/thomas_the_tank.alt" -> "thomas_the_tank.alt"
         *
         * @returns The reference's nickname (aka filename)
         *
         * @note A view is returned, so do not cache it without instantiating a hard-copy.
         */
        [[nodiscard]] constexpr auto GetNickName() const -> AZStd::string_view
        {
            return AZ::IO::PathView{ m_name.GetStringView() }.Filename().Native();
        }

        [[nodiscard]] constexpr auto IsValid() const -> bool
        {
            return !m_name.IsEmpty();
        }

    private:
        AZ::Name m_name{};
    };

    template<typename Tag>
    void TaggedResource<Tag>::Reflect(AZ::ReflectContext* context)
    {
        using ThisType = TaggedResource<Tag>;
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<ThisType>()->Field("Id", &TaggedResource<Tag>::m_name);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<ThisType>("TaggedResource", "");
            }
        }
    }

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

    using BankRef = TaggedResource<SoundBankTag>;
    AZ_TYPE_INFO_SPECIALIZE(BankRef, "{7AB9C911-22D9-4BC9-8C7F-9D7B31BF8282}");
    using BankRefContainer = AZStd::vector<BankRef>;

    using AudioEventId = TaggedResource<AudioEventTag>;
    AZ_TYPE_INFO_SPECIALIZE(AudioEventId, "{71A3D831-A821-41A6-95C8-FA76AFFDC5CE}");
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

    template<typename T, typename = void>
    struct HasTagMember : AZStd::false_type
    {
    };

    template<typename T>
    struct HasTagMember<T, AZStd::void_t<decltype(T::RefPath)>> : AZStd::true_type
    {
    };

    template<typename Tag>
    auto IsResource(Tag const&, ResourceRef const& resourceRef) -> bool
    {
        static_assert(
            HasTagMember<Tag>{}, "Unsupported tag. It must have a static member named 'RefPath'");
        return resourceRef.GetAsPath().IsRelativeTo(Tag::RefPath);
    }

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

namespace BopAudio
{

} // namespace BopAudio
