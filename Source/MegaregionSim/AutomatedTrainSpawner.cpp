#include "AutomatedTrainSpawner.h"
#include "Engine/World.h"
#include "TrainPawn.h"
#include "AITrainController.h"
#include "MegaregionGameMode.h"
#include "TimerManager.h"

void UAutomatedTrainSpawner::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Start polling for start menu completion
	InWorld.GetTimerManager().SetTimer(CheckMenuTimer, this, &UAutomatedTrainSpawner::CheckStartMenuComplete, 0.5f, true);
}

void UAutomatedTrainSpawner::CheckStartMenuComplete()
{
	AMegaregionGameMode* GameMode = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode && GameMode->bStartMenuComplete)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckMenuTimer);
		SpawnRealAITrains();
	}
}

void UAutomatedTrainSpawner::SpawnRealAITrains()
{
	// Spawn physical trains on the parallel track
	for (int32 i = 1; i <= 5; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation((i + 1) * 30000.0f, 0, 100.0f); // Spaced out every 30km
		FRotator SpawnRotation(0, 180.0f, 0); // Face opposite direction

		ATrainPawn* NewTrain = GetWorld()->SpawnActor<ATrainPawn>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewTrain)
		{
			NewTrain->bOnParallelTrack = true; // Use parallel track
			NewTrain->SpawnConsist(); // Let it spawn its cars

			// Give it an AI brain
			AAITrainController* Controller = GetWorld()->SpawnActor<AAITrainController>(SpawnLocation, SpawnRotation, SpawnParams);
			if (Controller)
			{
				Controller->Possess(NewTrain);
			}
		}
	}
}

void UAutomatedTrainSpawner::Tick(float DeltaTime)
{
	// No more virtual tick! Real physics runs on ATrainPawn now.
}
