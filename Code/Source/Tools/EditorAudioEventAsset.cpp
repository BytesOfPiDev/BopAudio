#include "Tools/EditorAudioEventAsset.h"

#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Serialization/SerializeContext.h"
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Serialization/EditContextConstants.inl>

namespace BopAudio
{
    void EditorAudioEventAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<EditorAudioEventAsset, AZ::Data::AssetData>()
                ->Version(0)
                ->Attribute(AZ::Edit::Attributes::EnableForAssetEditor, true)
                ->Field("Id", &EditorAudioEventAsset::m_name);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<EditorAudioEventAsset>("EditorAudioEventAsset", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &EditorAudioEventAsset::m_name, "Id", "");
            }
        }
    }

} // namespace BopAudio
