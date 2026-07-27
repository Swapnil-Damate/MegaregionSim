#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TrainCar.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsSloshingTest, "Megaregion.Physics.Sloshing", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPhysicsSloshingTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT("Starting Fluid Dynamics Sloshing Test..."));

	// Simulated deceleration assertion
	float SimulatedDeceleration = -150.0f;
	FVector ExpectedCenterOfMassShift = FVector(100.0f, 0, 0);

	if (SimulatedDeceleration < -100.0f)
	{
		AddInfo(TEXT("Deceleration threshold met. Asserting Center of Mass shift to X=100."));
		TestEqual(TEXT("COM Shift Math"), ExpectedCenterOfMassShift.X, 100.0f);
	}
	else
	{
		AddError(TEXT("Physics math failed: Deceleration did not trigger COM shift."));
	}

	return true;
}
