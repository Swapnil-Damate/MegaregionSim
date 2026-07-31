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
	if (!GraphGen || GraphGen->CityGraph.Num() == 0) return;

	for (const FCityNode& City : GraphGen->CityGraph)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = City.Location + FVector(0, 0, 100.0f); 
		FRotator SpawnRotation(0, 180.0f, 0);

		if (GraphGen->ExpressTrackForward)
		{
			float ClosestKey = GraphGen->ExpressTrackForward->FindInputKeyClosestToWorldLocation(SpawnLocation);
			SpawnLocation = GraphGen->ExpressTrackForward->GetLocationAtSplineInputKey(ClosestKey, ESplineCoordinateSpace::World);
			SpawnRotation = GraphGen->ExpressTrackForward->GetRotationAtSplineInputKey(ClosestKey, ESplineCoordinateSpace::World);
			SpawnLocation.Z += 100.0f;
		}

		ATrainPawn* NewTrain = GetWorld()->SpawnActor<ATrainPawn>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewTrain)
		{
			NewTrain->bOnParallelTrack = true;
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
	// No more virtual tick! Real physics runs on ATrainPawn now.
}
