#include "AutomatedTrainSpawner.h"
#include "Engine/World.h"
#include "TrainPawn.h"

void UAutomatedTrainSpawner::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Zero-Manual-Entry: Automatically spawn the Player's Train so you don't need Blueprints!
	FVector SpawnLocation(0.0f, 0.0f, 200.0f);
	FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
	InWorld.SpawnActor<ATrainPawn>(ATrainPawn::StaticClass(), SpawnLocation, SpawnRotation);

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
	for (FTrainFragment& Train : VirtualTrains)
	{
		float Acceleration = Train.Throttle * 10.0f; // 10 m/s^2 arbitrarily for virtual trains
		Train.Velocity += Acceleration * DeltaTime;
		Train.Position.X += Train.Velocity * DeltaTime;
	}
}
