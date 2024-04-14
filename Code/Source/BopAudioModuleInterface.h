
#include "AzCore/Memory/Memory_fwd.h"
#include "AzCore/RTTI/RTTIMacros.h"
#include "AzCore/RTTI/TypeInfoSimple.h"
#include "IGem.h"

namespace BopAudio
{
    class BopAudioModuleInterface : public CryHooksModule
    {
    public:
        AZ_TYPE_INFO_WITH_NAME_DECL(BopAudioModuleInterface)
        AZ_RTTI_NO_TYPE_INFO_DECL()
        AZ_CLASS_ALLOCATOR_DECL

        BopAudioModuleInterface();

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        [[nodiscard]] auto GetRequiredSystemComponents() const -> AZ::ComponentTypeList override;
    };
} // namespace BopAudio
