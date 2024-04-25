#pragma once

#include "AzCore/IO/Path/Path.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Name/Name.h"
#include "AzCore/RTTI/ReflectContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/std/containers/vector.h"
#include "IAudioSystem.h"

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
        AZ::Name m_name{};
        AZStd::string m_data{};
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

    class AudioObjectId
    {
    public:
        AZ_TYPE_INFO(AudioObjectId, "{70AFF12C-5FE9-4295-9FF1-E59F24F2429E}");

        AudioObjectId() = default;

        explicit constexpr AudioObjectId(Audio::TAudioObjectID objectId)
            : m_data(objectId){};

        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<AudioObjectId>()->Version(1);

                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<AudioObjectId>("Audio Object Id", "")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
                }
            }
        }

        [[nodiscard]] explicit constexpr operator Audio::TAudioObjectID() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr auto operator==(AudioObjectId const& rhs) const -> bool
        {
            return m_data == rhs.m_data;
        }

        [[nodiscard]] constexpr auto operator!=(AudioObjectId const& rhs) const -> bool
        {
            return !((*this) == rhs);
        }

    private:
        Audio::TAudioObjectID m_data;
    };

    static_assert(AZStd::is_pod_v<AudioObjectId>);

    static constexpr auto InvalidAudioObjectId = AudioObjectId{ INVALID_AUDIO_OBJECT_ID };
    static constexpr auto GlobalAudioObjectId = AudioObjectId{ 1 };

    class AudioEventId
    {
    public:
        AZ_TYPE_INFO(AudioEventId, "{AE4B6702-02F6-434D-AFD5-0B9995D58C18}");

        AudioEventId() = default;
        explicit AudioEventId(AZStd::string const& eventName)
            : m_data(Audio::AudioStringToID<Audio::TAudioEventID>(eventName.c_str())){};

        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<AudioEventId>()->Version(1);
                if (AZ::EditContext* editContext = serialize->GetEditContext())
                {
                    editContext->Class<AudioEventId>("AudioEventId", "")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
                }
            }
        }

        explicit constexpr AudioEventId(Audio::TAudioEventID crc)
            : m_data(crc){};

        constexpr auto operator==(AudioEventId const& rhs) const -> bool
        {
            return m_data == rhs.m_data;
        }

        constexpr auto operator!=(AudioEventId const& rhs) const -> bool
        {
            return !((*this) == rhs);
        }

        explicit constexpr operator Audio::TAudioEventID() const
        {
            return m_data;
        }

        constexpr auto operator<(AudioEventId const&) const -> bool = delete;
        constexpr auto operator>(AudioEventId const&) const -> bool = delete;

    private:
        Audio::TAudioEventID m_data;
    };

    static_assert(AZStd::is_pod_v<AudioEventId>);

    static constexpr auto InvalidAudioEventId{ AudioEventId{ 0 } };

    using AudioEventIdContainer = AZStd::vector<AudioEventId>;

} // namespace BopAudio

namespace AZStd
{
    template<>
    struct hash<BopAudio::AudioEventId>
    {
        [[nodiscard]] auto operator()(BopAudio::AudioEventId const& id) const -> size_t
        {
            static constexpr auto hasher{ AZStd::hash<Audio::TAudioEventID>{} };
            return hasher(static_cast<Audio::TAudioEventID>(id));
        }
    };

    template<>
    struct hash<BopAudio::AudioObjectId>
    {
        [[nodiscard]] auto operator()(BopAudio::AudioObjectId const& id) const -> size_t
        {
            static constexpr auto hasher{ AZStd::hash<Audio::TAudioObjectID>{} };
            return hasher(static_cast<Audio::TAudioObjectID>(id));
        }
    };

} // namespace AZStd

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
