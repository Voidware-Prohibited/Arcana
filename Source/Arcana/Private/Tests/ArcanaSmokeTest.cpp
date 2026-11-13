#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FArcanaModuleLoadedTest,
	"Arcana.Smoke.ModuleLoaded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FArcanaModuleLoadedTest::RunTest(const FString& Parameters)
{
	const bool bIsLoaded = FModuleManager::Get().IsModuleLoaded(TEXT("Arcana"));
	TestTrue(TEXT("Arcana plugin module should be loaded"), bIsLoaded);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
