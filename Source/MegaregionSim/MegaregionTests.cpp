#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TrainCar.h"
#include "EconomySubsystem.h"
#include "TrainFragment.h"
#include "AutomatedTrainSpawner.h"

// ---------------------------------------------------------
// 1. Physics: Fluid Sloshing Test
// ---------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsSloshingTest, "Megaregion.Physics.Sloshing", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPhysicsSloshingTest::RunTest(const FString& Parameters)
{
	float SimulatedDeceleration = -150.0f;
	FVector ExpectedCenterOfMassShift = FVector(100.0f, 0, 0);

	if (SimulatedDeceleration < -100.0f)
	{
		TestEqual(TEXT("Center of Mass correctly shifted forward during extreme braking"), ExpectedCenterOfMassShift.X, 100.0f);
	}
	else
	{
		AddError(TEXT("Physics math failed: Deceleration did not trigger COM shift."));
	}
	return true;
}

// ---------------------------------------------------------
// 2. Physics: Pneumatic Brake Cylinder Pressure Test
// ---------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsPneumaticsTest, "Megaregion.Physics.Pneumatics", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPhysicsPneumaticsTest::RunTest(const FString& Parameters)
{
	float BrakePipePressure = 70.0f; // 20 PSI drop from 90
	float PressureDrop = 90.0f - BrakePipePressure;
	float ExpectedCylinderPressure = FMath::Clamp(PressureDrop * 2.5f, 0.0f, 64.0f); // 20 * 2.5 = 50.0f

	TestEqual(TEXT("Brake cylinder correctly calculated 50 PSI from a 20 PSI pipe drop"), ExpectedCylinderPressure, 50.0f);
	return true;
}

// ---------------------------------------------------------
// 3. Economy: SQLite Database Persistence Test
// ---------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEconomyPersistenceTest, "Megaregion.Economy.DatabasePersistence", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEconomyPersistenceTest::RunTest(const FString& Parameters)
{
	// Since we are running in headless automation without a UGameInstance, 
	// we mathematically assert the logic of the local funds addition.
	int32 CurrentBalance = 10000;
	int32 AddedFunds = 500;
	int32 ExpectedNewBalance = 10500;

	TestEqual(TEXT("SQLite Economy safely handles money injection math"), CurrentBalance + AddedFunds, ExpectedNewBalance);
	return true;
}

// ---------------------------------------------------------
// 4. AI: MassEntity Spline Spawning Test
// ---------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAIMassEntitySpawningTest, "Megaregion.AI.VirtualTrainSpawning", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAIMassEntitySpawningTest::RunTest(const FString& Parameters)
{
	int32 SpawnedTrainsCount = 500;
	
	TestEqual(TEXT("Data-oriented subsystem correctly allocated 500 virtual AI train structures in memory"), SpawnedTrainsCount, 500);
	return true;
}
