#include "Engine/MiniAudioEngine.h"

#include "AzCore/Asset/AssetCommon.h"
#include "AzCore/Asset/AssetManager.h"
#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "Engine/MiniAudioEngineBus.h"
#include "IAudioSystem.h"
#include "MiniAudio/SoundAsset.h"

#include "Engine/ATLEntities_BopAudio.h"
#include "Engine/Common_BopAudio.h"
#include "Engine/ConfigurationSettings.h"

namespace BopAudio
{

    class AudioSystemImpl_BopAudio;

    MiniAudioEngine::MiniAudioEngine() = default;

    auto MiniAudioEngine::Initialize() -> bool
    {
        return true;
    }

    auto MiniAudioEngine::Unload() -> bool
    {
        m_soundBanks.clear();
        return true;
    }

    auto MiniAudioEngine::LoadSoundBank(Audio::SATLAudioFileEntryInfo* const fileEntryInfo) -> bool
    {
        m_soundBanks.push_back(fileEntryInfo);

        auto& soundBank = m_soundBanks.back();
        soundBank.Load();

        return true;
    }

    void MiniAudioEngine::LoadTrigger(AZ::rapidxml::xml_node<char>* triggerNode)
    {
        if (!triggerNode)
        {
            return;
        }
        /*
                auto const* const triggerNameNode{ triggerNode->first_attribute(BopAudioStrings::NameAttribute) };
                auto const* const triggerSoundName{ triggerNode->first_attribute(BopAudioStrings::SoundAttribute) };
                if (!triggerNameNode || !triggerSoundName)
                {
                    return;
                }

                auto const triggerName{ triggerNameNode->value() };
                auto const soundName{ triggerSoundName->value() };
                auto const triggerId{ Audio::AudioStringToID<BA_UniqueId>(triggerName) };
                auto const soundId{ Audio::AudioStringToID<BA_SoundId>(soundName) };

                m_triggers.push_back(triggerId);
                m_soundIds.push_back(soundId);

                AZ::IO::Path soundPath{ GetBanksRootPath() };
                soundPath /= triggerSoundName->value();

                AZ::Data::AssetId result{};
                AZ::Data::AssetCatalogRequestBus::BroadcastResult(
                    result,
                    &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
                    soundPath.c_str(),
                    AZ::AzTypeInfo<MiniAudio::SoundAsset>::Uuid(),
                    true);

                if (!result.IsValid())
                {
                    return;
                }

                MiniAudio::SoundDataAsset soundAsset{ AZ::Data::AssetManager::Instance().GetAsset<MiniAudio::SoundAsset>(
                    result, AZ::Data::AssetLoadBehavior::PreLoad) };

                soundAsset.QueueLoad();
                soundAsset.BlockUntilLoadComplete();
                // m_soundDatas.insert(soundAsset);

                //        m_sounds[soundId] = SoundPtr{ new ma_sound };
                m_triggerToSound[triggerId] = soundId;
            */
    }

    auto MiniAudioEngine::ActivateTrigger([[maybe_unused]] BA_UniqueId baId, [[maybe_unused]] BA_SoundId baSoundId) -> bool
    {
        return false;
    }

} // namespace BopAudio
