#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/document.h>

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/RTTI/TypeInfoSimple.h"

#include "Engine/Id.h"
#include "Engine/Tasks/Common.h"

namespace BopAudio
{

    class AudioObject;
    class AudioEventAsset : public AZ::Data::AssetData
    {
        friend class AudioEventAssetBuilderWorker;

    public:
        AZ_CLASS_ALLOCATOR_DECL;
        AZ_DISABLE_COPY_MOVE(AudioEventAsset);
        AZ_RTTI_NO_TYPE_INFO_DECL();
        AZ_TYPE_INFO_WITH_NAME_DECL(AudioEventAsset);

        static constexpr auto SourceExtension{ ".audioeventsource" };
        static constexpr auto ProductExtension{ ".audioevent" };
        static constexpr auto AssetGroup = "Sound";
        static constexpr auto AssetSubId = 1u;

        /*
         * Default constructor.
         *
         * TODO: Make protected after figuring out how to serialize classes with protected
         */
        AudioEventAsset() = default;
        ~AudioEventAsset() override = default;

        static void Reflect(AZ::ReflectContext* context);

        [[nodiscard]] auto GetId() const -> AudioEventId
        {
            return m_id;
        }

        [[nodiscard]] auto IsResource(ResourceRef const& resourceId) const -> bool
        {
            return m_id.IsValid() && (m_id.ToName() == resourceId.ToName());
        }

        void operator()(AudioObject& audioObject) const;

    protected:
        auto Execute(AudioObject& audioObject) const -> AZ::Outcome<void, char const*>;

    private:
        AudioEventId m_id{};
        TaskContainer m_tasks{};
    };

    using AudioEvents = AZStd::vector<AudioEventAsset>;
    using AudioEventAssetDataPtr = AZ::Data::Asset<AudioEventAsset>;
} // namespace BopAudio
