#pragma once

#include "Clients/BootstrapFixture.h"

namespace BopAudioTests
{
    class SimpleProjectFixture : public BootstrapFixture
    {
    public:
        void SetUp() override;
        void TearDown() override;
    };
} // namespace BopAudioTests
