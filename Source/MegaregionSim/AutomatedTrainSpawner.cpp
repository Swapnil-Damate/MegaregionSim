#include "AutomatedTrainSpawner.h"
#include "MassEntitySubsystem.h"
#include "TrainFragment.h"
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
	UMassEntitySubsystem* MassSubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	if (!MassSubsystem) return;

	TArray<const UScriptStruct*> Fragments;
	Fragments.Add(FTrainFragment::StaticStruct());
	
	FMassArchetypeHandle TrainArchetype = MassSubsystem->CreateArchetype(Fragments);

	for (int32 i = 0; i < Count; ++i)
	{
		MassSubsystem->CreateEntity(TrainArchetype);
	}
}
