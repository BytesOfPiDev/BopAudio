#pragma once

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/RTTI/ReflectContext.h"
#include <AzFramework/Asset/GenericAssetHandler.h>

namespace BopAudio
{
    class EditorAudioEventAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(EditorAudioEventAsset, AZ::SystemAllocator);
        AZ_DISABLE_COPY_MOVE(EditorAudioEventAsset);
        AZ_RTTI(
            EditorAudioEventAsset, "{BA4586EF-DF59-4B2B-92F4-12F9169A0BFC}", AZ::Data::AssetData);

        static constexpr auto SourceExtension{ ".editoraudioevent" };
        static constexpr auto AssetGroup = "Sound";
        static constexpr auto AssetSubId = 1u;

        EditorAudioEventAsset() = default;
        ~EditorAudioEventAsset() override = default;

        static void Reflect(AZ::ReflectContext* context);

    private:
        AZStd::string m_name;
    };

    using EditorAudioEventAssetHandler = AzFramework::GenericAssetHandler<EditorAudioEventAsset>;
} // namespace BopAudio
