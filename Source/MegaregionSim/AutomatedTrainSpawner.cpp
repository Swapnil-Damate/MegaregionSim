#include "AutomatedTrainSpawner.h"
#include "MassEntitySubsystem.h"
#include "TrainFragment.h"
#include "Engine/World.h"

void UAutomatedTrainSpawner::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
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
		FMassEntityHandle NewEntity = MassSubsystem->CreateEntity(TrainArchetype);
		
		if (MassSubsystem->IsEntityValid(NewEntity))
		{
			FTrainFragment* TrainData = MassSubsystem->GetFragmentDataPtr<FTrainFragment>(NewEntity);
			if (TrainData)
			{
				// Space them out 500 meters apart randomly
				TrainData->Position = FVector(FMath::RandRange(-250000.0f, 250000.0f), 0, 0);
				TrainData->Velocity = 0.0f;
				TrainData->Throttle = FMath::RandRange(0.1f, 1.0f); // Constant AI throttle
			}
		}
	}
}
