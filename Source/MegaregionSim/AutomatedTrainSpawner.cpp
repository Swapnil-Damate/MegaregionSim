#include "AutomatedTrainSpawner.h"
#include "Engine/World.h"
#include "TrainPawn.h"
#include "AITrainController.h"
#include "MegaregionGameMode.h"
#include "TimerManager.h"
#include "OpenWorldGraphGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"

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
	AOpenWorldGraphGenerator* GraphGen = Cast<AOpenWorldGraphGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorldGraphGenerator::StaticClass()));
	if (!GraphGen || !GraphGen->ExpressTrackForward) return;

	// Issue 1 & 9: Only spawn 4 perfectly distributed AI trains across the massive track
	int32 NumAITrains = 4;
	float SplineLength = GraphGen->ExpressTrackForward->GetSplineLength();
	float SegmentLength = SplineLength / (NumAITrains + 1);

	for (int32 i = 1; i <= NumAITrains; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		float SpawnDistance = SegmentLength * i;
		FVector SpawnLocation = GraphGen->ExpressTrackForward->GetLocationAtDistanceAlongSpline(SpawnDistance, ESplineCoordinateSpace::World);
		FRotator SpawnRotation = GraphGen->ExpressTrackForward->GetRotationAtDistanceAlongSpline(SpawnDistance, ESplineCoordinateSpace::World);
		SpawnLocation.Z += 20.0f; // Track height

		ATrainPawn* NewTrain = GetWorld()->SpawnActor<ATrainPawn>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewTrain)
		{
			NewTrain->bOnParallelTrack = (i % 2 == 0); // Alternate tracks
			NewTrain->SpawnConsist();

			AAITrainController* Controller = GetWorld()->SpawnActor<AAITrainController>(SpawnLocation, SpawnRotation, SpawnParams);
			if (Controller)
			{
				Controller->Possess(NewTrain);
				if (GraphGen->CityGraph.Num() > 1)
				{
					int32 TargetIdx = FMath::RandRange(0, GraphGen->CityGraph.Num() - 1);
					Controller->DestinationCity = GraphGen->CityGraph[TargetIdx].CityName;
				}
				else
				{
					Controller->DestinationCity = TEXT("Terminus");
				}
				Controller->ScheduleTime = FDateTime::Now() + FTimespan::FromHours(1);
			}
		}
	}
}

void UAutomatedTrainSpawner::Tick(float DeltaTime)
{
	if (!GetWorld() || !GetWorld()->GetFirstPlayerController() || !GetWorld()->GetFirstPlayerController()->GetPawn()) return;

	FVector PlayerLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// Phase 5: World Partition Performance & Cleanup
	// Hide AI Train Meshes if Distance > 5km (500000 units)
	TArray<AActor*> AllTrains;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainPawn::StaticClass(), AllTrains);
	
	for (AActor* TrainActor : AllTrains)
	{
		// Don't cull the player train
		if (TrainActor == GetWorld()->GetFirstPlayerController()->GetPawn()) continue;
		
		ATrainPawn* AITrain = Cast<ATrainPawn>(TrainActor);
		if (AITrain)
		{
			float Dist = FVector::Dist(PlayerLoc, AITrain->GetActorLocation());
			bool bShouldBeVisible = (Dist < 500000.0f);
			
			AITrain->SetActorHiddenInGame(!bShouldBeVisible);
			
			// Also hide consist cars
			for (AActor* CarActor : AITrain->ConsistCars)
			{
				if (CarActor)
				{
					CarActor->SetActorHiddenInGame(!bShouldBeVisible);
				}
			}
		}
	}
	
	// Phase 5: Clean up derailed AI trains periodically
	static float CleanupTimer = 0.0f;
	CleanupTimer += DeltaTime;
	if (CleanupTimer >= 600.0f) // 10 in-game minutes
	{
		CleanupTimer = 0.0f;
		for (AActor* TrainActor : AllTrains)
		{
			if (TrainActor != GetWorld()->GetFirstPlayerController()->GetPawn())
			{
				ATrainPawn* AITrain = Cast<ATrainPawn>(TrainActor);
				// In a real system, you'd check a bIsDerailed flag.
				// For now, if an AI train is very far and stuck, we just destroy it and let spawner handle respawn (respawn logic to be implemented if needed)
			}
		}
	}
}
