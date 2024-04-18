#pragma once

#include "AudioAllocators.h"
#include "AzCore/IO/Path/Path.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Name/Name.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/std/containers/vector.h"

#include "Engine/ConfigurationSettings.h"
#include "IAudioInterfacesCommonData.h"
#include <AzCore/Serialization/EditContextConstants.inl>

namespace BopAudio
{
    /* clang-format off */
    struct AudioEventTag{ static constexpr AZ::IO::PathView RefPath{EventsAlias}; };
    struct AudioObjectTag{};
    struct SoundTag{ static constexpr AZ::IO::PathView RefPath{SoundsAlias}; };
    struct SoundBankTag{ static constexpr AZ::IO::PathView RefPath{BanksAlias}; };
    /* clang-format on */

    class ResourceRefBase
    {
    public:
        AZ_DEFAULT_COPY_MOVE(ResourceRefBase);
        AZ_CLASS_ALLOCATOR(ResourceRefBase, AZ::SystemAllocator);
        AZ_TYPE_INFO(ResourceRefBase, "{7ED165C7-85D0-46F3-8D11-18B19DDA3A2E}");

        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ResourceRefBase>()
                    ->Version(1)
                    ->Field("Id", &ResourceRefBase::m_name)
                    ->Field("path", &ResourceRefBase::m_data);

                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<ResourceRefBase>("ResourceRefBase", "")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "BopAudio")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(
                            AZ::Edit::UIHandlers::Default, &ResourceRefBase::m_data, "Path", "");
                }
            }
        }

        ResourceRefBase() = default;
        ~ResourceRefBase() = default;

        explicit ResourceRefBase(AZStd::string_view resourceName)
            : m_name{ resourceName } {};

        explicit ResourceRefBase(AZ::Name resourceName)
            : m_name{ AZStd::move(resourceName) } {};

        [[nodiscard]] explicit operator size_t() const
        {
            return m_name.GetHash();
        }

        auto operator==(ResourceRefBase const& other) const -> bool
        {
            return m_name == other.m_name;
        }

        auto operator==(AZ::Name const& resourceName) const -> bool
        {
            return m_name == resourceName;
        };

        [[nodiscard]] auto ToName() const -> AZ::Name
        {
            return m_name;
        }

        [[nodiscard]] auto GetCStr() const -> char const*
        {
            return m_name.GetCStr();
        }

        [[nodiscard]] auto c_str() const -> char const*
        {
            return GetCStr();
        }

        [[nodiscard]] auto GetHash() const -> AZ::u32
        {
            return m_name.GetHash();
        }

        [[nodiscard]] auto IsValid() const -> bool
        {
            return !m_name.IsEmpty();
        }

    private:
        AZ::Name m_name{ "test" };
        AZStd::string m_data;
    };

    template<typename Tag>
    class TaggedResource final : public ResourceRefBase
    {
        using Self = TaggedResource<Tag>;

    public:
        AZ_DEFAULT_COPY_MOVE(TaggedResource<Tag>);
        AZ_CLASS_ALLOCATOR(TaggedResource<Tag>, AZ::SystemAllocator);

        constexpr TaggedResource() = default;
        ~TaggedResource() = default;

        explicit constexpr TaggedResource(AZStd::string_view resourceName)
            : ResourceRefBase(AZ::Name{ resourceName }){};
    };

    using ResourceRef = TaggedResource<void>;

    using SoundRef = TaggedResource<SoundTag>;
    using BankRef = TaggedResource<SoundBankTag>;
    using BankRefContainer = AZStd::vector<BankRef>;

    template<typename Tag = void>
    class TaggedId
    {
    public:
        AZ_DEFAULT_COPY_MOVE(TaggedId);

        constexpr TaggedId() = default;
        constexpr TaggedId(AZ::u32 id)
            : m_value{ id } {};
        ~TaggedId() = default;

        [[nodiscard]] explicit constexpr operator size_t() const
        {
            return m_value;
        }

        [[nodiscard]] explicit constexpr operator AZ::u32() const
        {
            return m_value;
        }

        [[nodiscard]] constexpr auto operator==(TaggedId const& other) -> bool
        {
            return m_value == other.m_value;
        }

        [[nodiscard]] constexpr auto operator!=(TaggedId const& other) -> bool
        {
            return !(m_value == other.m_value);
        }

        [[nodiscard]] constexpr auto IsValid() const
        {
            return m_value != 0;
        }

    private:
        AZ::u32 m_value;
    };

    using AudioObjectId = TaggedId<AudioObjectTag>;
    static constexpr auto InvalidAudioObjectId = AudioObjectId{ INVALID_AUDIO_OBJECT_ID };
    static constexpr auto GlobalAudioObjectId = AudioObjectId{ 1 };

    using AudioEventId = Audio::TAudioEventID;
    static constexpr auto InvalidAudioEventId{ 0 };

    using AudioEventIdContainer = AZStd::vector<AudioEventId>;

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
        BopAudio::SoundRef, "{58C17C6F-BE02-420A-B7D4-5301DF1E2234}", "SoundResourceRef");
} // namespace AZ
