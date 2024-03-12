#pragma once

#include "AzCore/IO/Path/Path.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"

#include "AzCore/Name/Name.h"
#include "AzCore/std/containers/vector.h"
#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{
    /* clang-format off */
    struct AudioEventTag{ static constexpr AZ::IO::PathView RefPath{EventsAlias}; };
    struct SoundTag{ static constexpr AZ::IO::PathView RefPath{SoundsAlias}; };
    struct SoundBankTag{ static constexpr AZ::IO::PathView RefPath{BanksAlias}; };
    /* clang-format on */

    class ResourceRefBase
    {
    public:
        AZ_CLASS_ALLOCATOR(ResourceRefBase, AZ::SystemAllocator);
        AZ_TYPE_INFO(ResourceRefBase, "{7ED165C7-85D0-46F3-8D11-18B19DDA3A2E}");

        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ResourceRefBase>()->Version(1)->Field(
                    "Id", &ResourceRefBase::m_name);
                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<ResourceRefBase>("ResourceRefBase", "");
                }
            }
        }

        ResourceRefBase() = default;

        explicit ResourceRefBase(AZStd::string_view resourceName)
            : m_name{ resourceName } {};

        explicit ResourceRefBase(AZ::Name resourceName)
            : m_name{ AZStd::move(resourceName) } {};

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            return m_name.GetHash();
        }

        constexpr auto operator==(ResourceRefBase const& other) const -> bool
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

        [[nodiscard]] constexpr auto IsValid() const -> bool
        {
            return !m_name.IsEmpty();
        }

    private:
        AZ::Name m_name{};
    };

    template<typename T, typename = void>
    struct HasTagMember : AZStd::false_type
    {
    };

    template<typename T>
    struct HasTagMember<T, AZStd::void_t<decltype(T::RefPath)>> : AZStd::true_type
    {
    };

    template<typename Tag = void>
    class TaggedResource final : public ResourceRefBase
    {
        using Self = TaggedResource<Tag>;

        template<typename>
        friend void ReflectTaggedResource(AZ::ReflectContext*);

    public:
        AZ_DEFAULT_COPY_MOVE(TaggedResource<Tag>);
        AZ_CLASS_ALLOCATOR(TaggedResource<Tag>, AZ::SystemAllocator);

        constexpr TaggedResource() = default;
        ~TaggedResource() = default;

        [[nodiscard]] static constexpr auto IsResource(AZStd::string_view resource) -> bool
        {
            if constexpr (HasTagMember<Tag>{})
            {
                return AZ::IO::PathView{ resource }.IsRelativeTo(Tag::RefPath);
            }
            else
            {
                return false;
            }
        }

        explicit constexpr TaggedResource(AZStd::string_view resourceName)
            : ResourceRefBase(AZ::Name{ resourceName }){};
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

} // namespace BopAudio

namespace AZ
{
    AZ_TYPE_INFO_TEMPLATE_WITH_NAME(
        BopAudio::TaggedResource,
        "ResourceTag",
        "{AE00E464-11C7-42FC-80FB-CD8CD43C4779}",
        AZ_TYPE_INFO_TYPENAME);

    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        BopAudio::BankRef, "{53D23BB0-5F71-49ED-B507-7244679746A5}", "BankResourceRef");

    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        BopAudio::AudioEventId, "{92D29F86-1B00-4A97-9B5B-E23B8C7C4EAD}", "AudioEventId");

    AZ_TYPE_INFO_SPECIALIZE_WITH_NAME(
        BopAudio::SoundRef, "{58C17C6F-BE02-420A-B7D4-5301DF1E2234}", "SoundResourceRef");
} // namespace AZ
