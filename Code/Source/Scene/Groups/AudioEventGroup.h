#pragma once

#include <AzCore/RTTI/RTTI.h>

#include "AzCore/Memory/SystemAllocator.h"
#include "SceneAPI/SceneCore/DataTypes/Groups/IGroup.h"

namespace BopAudioScene
{
    class AudioEventGroup : public AZ::SceneAPI::DataTypes::IGroup
    {
    public:
        AZ_RTTI(
            AudioEventGroup,
            "{C2093463-996B-43A9-B9F8-30BF355248BD}",
            AZ::SceneAPI::DataTypes::IGroup);
        AZ_CLASS_ALLOCATOR(AudioEventGroup, AZ::SystemAllocator);
        AZ_DEFAULT_COPY_MOVE(AudioEventGroup);

        AudioEventGroup();
        ~AudioEventGroup() override;

        static void Reflect(AZ::ReflectContext* context);

        void SetName(AZStd::string const& name);
        void SetName(AZStd::string&& name);
        [[nodiscard]] auto GetName() const -> AZStd::string const& override;
        [[nodiscard]] auto GetId() const -> const AZ::Uuid& override;

        auto GetRuleContainer() -> AZ::SceneAPI::Containers::RuleContainer& override;
        [[nodiscard]] auto GetRuleContainerConst() const
            -> const AZ::SceneAPI::Containers::RuleContainer& override;

        [[nodiscard]] auto GetGraphLogRoot() const -> AZStd::string const&;
        [[nodiscard]] auto DoesLogGraph() const -> bool;

        [[nodiscard]] auto DoesLogProcessingEvents() const -> bool;
        void ShouldLogProcessingEvents(bool state);

    protected:
        AZ::SceneAPI::Containers::RuleContainer m_ruleContainer;
        AZStd::string m_groupName;
        AZStd::string m_graphLogRoot;
        AZ::Uuid m_id;
        bool m_logProcessingEvents{};

        static char const* const s_disabledOption;
    };
} // namespace BopAudioScene
