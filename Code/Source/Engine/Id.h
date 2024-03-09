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
    struct AudioEventTag{ static constexpr AZ::IO::PathView RefPath{EventsPath}; };
    struct SoundTag{ static constexpr AZ::IO::PathView RefPath{SoundSourcePath}; };
    struct SoundBankTag{ static constexpr AZ::IO::PathView RefPath{DefaultBanksPath}; };
    /* clang-format on */

    template<typename Tag>
    class TaggedResource;

    template<typename Tag = void>
    class TaggedResource
    {
        using Self = TaggedResource<Tag>;

        template<typename>
        friend void ReflectTaggedResource(AZ::ReflectContext*);

    public:
        AZ_DEFAULT_COPY_MOVE(TaggedResource);
        AZ_TYPE_INFO(TaggedResource, "{AE00E464-11C7-42FC-80FB-CD8CD43C4779}");

        TaggedResource() = default;
        ~TaggedResource() = default;

        explicit TaggedResource(AZStd::string_view resourceName)
            : m_name{ resourceName } {};

        explicit TaggedResource(AZ::Name resourceName)
            : m_name{ AZStd::move(resourceName) } {};

        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<Self>()->Field("Id", &Self::m_name);
                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<Self>("TaggedResource", "");
                }
            }
        }

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            return m_name.GetHash();
        }

        constexpr auto operator==(TaggedResource const& other) const -> bool
        {
            return m_name == other.m_name;
        }

        constexpr auto operator==(AZ::Name const& resourceName) const -> bool
        {
            return m_name == resourceName;
        };

        constexpr auto operator<(Self const& other) const -> bool
        {
            return m_name < other.m_name;
        }

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

        [[nodiscard]] constexpr auto IsValid() const -> bool
        {
            return !m_name.IsEmpty();
        }

    private:
        AZ::Name m_name{};
    };

    using ResourceRef = TaggedResource<>;

    using SoundRef = TaggedResource<SoundTag>;

    using BankRef = TaggedResource<SoundBankTag>;
    using BankRefContainer = AZStd::vector<BankRef>;

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
    using AudioObjectId = AZ::u32;

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

        return AZ::IO::PathView{ resourceRef.GetCStr() }.IsRelativeTo(Tag::RefPath);
    }

} // namespace BopAudio
