#pragma once

#include "Clients/BaseTestFixture.h"

namespace BopAudioTests
{
    class BootstrapFixture : public BaseAudioTestFixture
    {
    public:
        void SetUp() override;

        void TearDown() override;
    };
} // namespace BopAudioTests
