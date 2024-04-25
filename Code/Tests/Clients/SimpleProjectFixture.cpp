#include "Clients/SimpleProjectFixture.h"

#include "Clients/BootstrapFixture.h"
#include <IAudioInterfacesCommonData.h>

namespace BopAudioTests
{
    void SimpleProjectFixture::SetUp()
    {
        BootstrapFixture::SetUp();
    }

    void SimpleProjectFixture::TearDown()
    {
        BootstrapFixture::TearDown();
    }
} // namespace BopAudioTests
