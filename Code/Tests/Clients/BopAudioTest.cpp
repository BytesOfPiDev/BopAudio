#include "AzCore/Asset/AssetTypeInfoBus.h"

#include "Clients/AudioEventAsset.h"
#include "Clients/BootstrapFixture.h"

using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace BopAudioTests
{
    TEST_F(BaseAudioTestFixture, SANITY_CHECK)
    {
    }

    TEST_F(BootstrapFixture, AudioEventAssetAssetHandler_IsConnected)
    {
        EXPECT_EQ(
            AZ::AssetTypeInfoBus::GetNumOfEventHandlers(
                AZ::AzTypeInfo<BopAudio::AudioEventAsset>::Uuid()),
            1);
    }
} // namespace BopAudioTests
