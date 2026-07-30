#include "AutomatedTrainSpawner.h"
#include "Engine/World.h"
#include "TrainPawn.h"

void UAutomatedTrainSpawner::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Removing the hardcoded SpawnActor because our VisualTest.py Python script
	// now spawns the Blueprints automatically!

	SpawnVirtualTrains(500); // 500 AI trains mathematically simulated in the background!
}

void UAutomatedTrainSpawner::SpawnVirtualTrains(int32 Count)
{
	VirtualTrains.Empty(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		FTrainFragment NewTrain;
		NewTrain.Position = FVector(FMath::RandRange(-250000.0f, 250000.0f), 0, 0);
		NewTrain.Velocity = 0.0f;
		NewTrain.Throttle = FMath::RandRange(0.1f, 1.0f); // Constant AI throttle
		VirtualTrains.Add(NewTrain);
	}
}

void UAutomatedTrainSpawner::Tick(float DeltaTime)
{
	// Data-oriented array iteration for CPU performance (Replaces experimental MassEntity plugin)
	const float MaxSpeedMs = 55.5f; // 200 km/h cap
	const float WorldExtent = 500000.0f; // Wrap at 5km to stay in generated world

	for (FTrainFragment& Train : VirtualTrains)
	{
		float Acceleration = Train.Throttle * 2.0f; // Gentle acceleration for virtual trains
		Train.Velocity = FMath::Min(Train.Velocity + Acceleration * DeltaTime, MaxSpeedMs);
		Train.Position.X += Train.Velocity * DeltaTime;

		// Wrap position so virtual trains stay within the generated world extent
		if (Train.Position.X > WorldExtent)  Train.Position.X -= WorldExtent * 2.0f;
		if (Train.Position.X < -WorldExtent) Train.Position.X += WorldExtent * 2.0f;
	}
}
