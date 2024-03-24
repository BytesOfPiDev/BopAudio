#include "AsiTestEnvironment.h"

#include "AzCore/Asset/AssetManagerComponent.h"
#include "AzCore/IO/Streamer/StreamerComponent.h"
#include "AzCore/Jobs/JobManagerComponent.h"
#include "AzFramework/Application/Application.h"
#include "AzFramework/Asset/AssetCatalogComponent.h"
#include "AzFramework/Asset/AssetSystemComponent.h"
#include "AzFramework/Entity/GameEntityContextComponent.h"
#include "AzTest/GemTestEnvironment.h"
#include "Clients/BopAudioSystemComponent.h"

namespace BopAudioTests
{

    class AsiTestApplication : public AzFramework::Application
    {
    public:
        AZ_DISABLE_COPY_MOVE(AsiTestApplication);

        AsiTestApplication() = default;

        ~AsiTestApplication() override = default;

        auto GetRequiredSystemComponents() const -> AZ::ComponentTypeList override
        {
            auto requiredComponents = AzFramework::Application::GetRequiredSystemComponents();

            requiredComponents.insert(
                requiredComponents.end(),
                { azrtti_typeid<AZ::StreamerComponent>(),
                  azrtti_typeid<AZ::AssetManagerComponent>(),
                  azrtti_typeid<AZ::JobManagerComponent>(),
                  azrtti_typeid<AzFramework::AssetCatalogComponent>(),
                  azrtti_typeid<AzFramework::GameEntityContextComponent>(),
                  azrtti_typeid<AzFramework::AssetSystem::AssetSystemComponent>() });

            return requiredComponents;
        }
    };

    void AsiTestEnvironment::AddGemsAndComponents()
    {
        AddDynamicModulePaths({ "LmbrCentral" });
        AddComponentDescriptors({ BopAudio::BopAudioSystemComponent::CreateDescriptor() });
    }

    auto AsiTestEnvironment::CreateApplicationInstance() -> AZ::ComponentApplication*
    {
        return AZ::Test::GemTestEnvironment::CreateApplicationInstance();
    }
} // namespace BopAudioTests
