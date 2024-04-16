#include "Engine/Tasks/PlaySound.h"

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/RTTI/RTTIMacros.h"
#include "AzCore/Serialization/EditContextConstants.inl"

#include "Engine/AudioObject.h"
#include "Engine/Sound.h"
#include "Engine/Tasks/AudioTaskBase.h"

namespace BopAudio
{
    void PlaySoundTask::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<PlaySoundTask, IAudioTask>()->Version(6)->Field(
                "SoundSource", &PlaySoundTask::m_soundSource);

            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                editContext->Class<PlaySoundTask>("PlaySoundTask", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "BopAudio/Tasks")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->Attribute(AZ::Edit::Attributes::NameLabelOverride, "Target")
                    ->Attribute(
                        AZ::Edit::Attributes::Visibility,
                        AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &PlaySoundTask::m_soundSource,
                        "Sound Source",
                        "");
            }
        }
    }

    void PlaySoundTask::operator()(AudioObject& audioObject) const
    {
        auto soundToPlay = SoundInstance(m_soundSource);
        if (!soundToPlay.IsValid())
        {
            AZ_Error(
                "PlaySoundTask",
                false,
                "Failed to create sound instance with resource '%s'",
                m_soundSource.GetResourceId().GetCStr());

            return;
        }

        soundToPlay.SetVolume(1.0f);
        audioObject.PlaySound(AZStd::move(soundToPlay));

        AZ_Info("PlaySoundTask", "Play: [%s]", m_soundSource.GetResourceId().c_str());
    }

} // namespace BopAudio
