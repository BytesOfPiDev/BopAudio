#pragma once

#include <AzCore/UnitTest/UnitTest.h>
#include <AzTest/AzTest.h>

#include "AzCore/Name/NameDictionary.h"
#include "Engine/AudioSystemImpl_BopAudio.h"

class BaseAudioTestFixture
    : public UnitTest::TraceBusRedirector
    , public testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    auto GetAudioSystemImpl() -> BopAudio::AudioSystemImpl_bopaudio&
    {
        return m_impl;
    }

    auto GetAudioSystemImpl() const -> BopAudio::AudioSystemImpl_bopaudio const&
    {
        return m_impl;
    }

private:
    BopAudio::AudioSystemImpl_bopaudio m_impl{};
    AZ::NameDictionary m_dictionary{};
};
