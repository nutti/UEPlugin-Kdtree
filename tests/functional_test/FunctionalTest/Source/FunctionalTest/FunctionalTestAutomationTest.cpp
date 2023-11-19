#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFuntionalTestSyncVersion, "Kdtree.FunctionalTest.SyncVersion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

bool TestCommon(FAutomationTestBase* AuctionmationTest, UBlueprint* Blueprint)
{
	AuctionmationTest->TestNotNull(TEXT("Blueprint should not be null"), Blueprint);

	UClass* GeneratedClass = Blueprint->GeneratedClass;
	UWorld* World = GEngine->GetWorldContexts()[0].World();
	AActor* Actor = World->SpawnActor<AActor>(GeneratedClass);
	AuctionmationTest->TestNotNull(TEXT("Actor should not be null"), Actor);

	for (TFieldIterator<UFunction> It(GeneratedClass); It; ++It)
	{
		UFunction* Function = *It;
		if (Function->GetFName().ToString().StartsWith("Test_"))
		{
			struct FParameters
			{
				int32 ReturnValue = 0;
			};
			int32 Expect = 1;
			FParameters Params;
			Actor->ProcessEvent(Function, &Params);
			AuctionmationTest->AddInfo(FString::Format(TEXT("Run '{0}'"), {Function->GetFName().ToString()}));
			AuctionmationTest->TestEqual(TEXT("The return value is incorrect"), Params.ReturnValue, Expect);
		}
	}

	return true;
}

bool FFuntionalTestSyncVersion::RunTest(const FString& Parameters)
{
	UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/FunctionalTest/SyncVersion.SyncVersion"));

	return TestCommon(this, Blueprint);
}
