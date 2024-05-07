#include "AzCore/Name/NameDictionary.h"
#include "Clients/BopAudioSystemComponent.h"

#include "AzCore/Asset/AssetManagerBus.h"
#include "AzCore/Asset/AssetTypeInfoBus.h"
#include "AzCore/IO/FileIO.h"
#include "AzCore/UnitTest/UnitTest.h"
#include "AzFramework/IO/LocalFileIO.h"
#include "AzTest/AzTest.h"
#include "IAudioSystem.h"
#include "IAudioSystemImplementation.h"
#include "ISystem.h"
#include "Mocks/IConsoleMock.h"
#include "Mocks/ISystemMock.h"

using ::testing::NiceMock;
namespace BopAudioTests
{
    struct TestNameDictionary
    {
        TestNameDictionary()
        {
            m_interface.Register(&m_dictionary);
        }

        ~TestNameDictionary()
        {
            m_interface.Unregister(&m_dictionary);
        }

        AZ::Interface<AZ::NameDictionary> m_interface{};
        AZ::NameDictionary m_dictionary{};
    };

    class AsiTestEnvironment : public UnitTest::TraceBusHook
    {
    public:
        AZ_DISABLE_COPY_MOVE(AsiTestEnvironment);

        AsiTestEnvironment() = default;
        ~AsiTestEnvironment() override = default;

    protected:
        struct MockHolder
        {
            NiceMock<ConsoleMock> m_console;
            NiceMock<SystemMock> m_system;
        };

        void SetupEnvironment() override
        {
            UnitTest::TraceBusHook::SetupEnvironment();

            m_mocks = new MockHolder{};
            m_stubEnv.pConsole = &m_mocks->m_console;
            m_stubEnv.pSystem = &m_mocks->m_system;

            ASSERT_EQ(AZ::IO::FileIOBase::GetInstance(), nullptr);

            m_fileIo = AZStd::make_unique<AZ::IO::LocalFileIO>();
            AZ::IO::FileIOBase::SetInstance(m_fileIo.get());

            m_stubEnv.pFileIO = m_fileIo.get();
            gEnv = &m_stubEnv;

            m_nameDictionary = AZStd::make_unique<TestNameDictionary>();

            BopAudio::BopAudioSystemComponent::RegisterFileAliases();

            AZ::Data::AssetCatalogRequestBus::GetOrCreateContext();
            AZ::Data::AssetManagerNotificationBus::GetOrCreateContext();
            AZ::AssetTypeInfoBus::GetOrCreateContext();
            Audio::AudioSystemImplementationRequestBus::GetOrCreateContext();
            Audio::Gem::EngineRequestBus::GetOrCreateContext();
        }

        void TeardownEnvironment() override
        {
            UnitTest::TraceBusHook::TeardownEnvironment();
            m_fileIo = nullptr;

            m_nameDictionary = nullptr;

            delete m_mocks;
            m_mocks = nullptr;
        }

    private:
        SSystemGlobalEnvironment m_stubEnv{};
        MockHolder* m_mocks{ nullptr };
        AZStd::unique_ptr<AZ::IO::FileIOBase> m_fileIo{};
        AZStd::unique_ptr<TestNameDictionary> m_nameDictionary{};
    };
} // namespace BopAudioTests

AZ_UNIT_TEST_HOOK(new BopAudioTests::AsiTestEnvironment);
