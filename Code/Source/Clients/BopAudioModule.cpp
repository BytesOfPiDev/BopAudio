
#include <BopAudio/BopAudioTypeIds.h>
#include <BopAudioModuleInterface.h>

namespace BopAudio
{
    class BopAudioModule : public BopAudioModuleInterface
    {
    public:
        AZ_RTTI(BopAudioModule, BopAudioModuleTypeId, BopAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(BopAudioModule, AZ::SystemAllocator);
    };
} // namespace BopAudio

AZ_DECLARE_MODULE_CLASS(Gem_BopAudio, BopAudio::BopAudioModule)
