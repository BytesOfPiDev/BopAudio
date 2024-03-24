#pragma once

#include "AzTest/GemTestEnvironment.h"

namespace BopAudioTests
{
    class AsiTestEnvironment : public AZ::Test::GemTestEnvironment
    {
    public:
        AZ_DEFAULT_COPY_MOVE(AsiTestEnvironment);

        AsiTestEnvironment() = default;
        ~AsiTestEnvironment() override = default;

        void AddGemsAndComponents() override;
        auto CreateApplicationInstance() -> AZ::ComponentApplication* override;
    };
} // namespace BopAudioTests
