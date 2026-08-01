#include "AITrainController.h"
#include "TrainPawn.h"
#include "Kismet/GameplayStatics.h"
#include "MegaregionZoningGenerator.h"
#include "LevelCrossing.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"

AAITrainController::AAITrainController()
{
	PrimaryActorTick.bCanEverTick = true;
	TargetSpeedKmh = 60.0f;
	SafeFollowingDistance = 20000.0f; // 2km
}

void AAITrainController::BeginPlay()
{
	Super::BeginPlay();

	// Cache all signals once at startup rather than scanning every tick
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARailwaySignal::StaticClass(), CachedSignals);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelCrossing::StaticClass(), CachedCrossings);
	
	// Refresh cache every 10 seconds in case new signals are dynamically spawned by chunks
	GetWorldTimerManager().SetTimer(SignalCacheTimer, this, &AAITrainController::RefreshSignalCache, 10.0f, true);
}

void AAITrainController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledTrain = Cast<ATrainPawn>(InPawn);
}

void AAITrainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledTrain) return;

	ScanForSignals();
}

void AAITrainController::RefreshSignalCache()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARailwaySignal::StaticClass(), CachedSignals);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelCrossing::StaticClass(), CachedCrossings);
}

void AAITrainController::CleanupCrash()
{
	if (ControlledTrain)
	{
		ControlledTrain->Destroy();
	}
	Destroy();
}

void AAITrainController::ScanForSignals()
{
	if (!ControlledTrain) return;

	FVector2D Loc2D(ControlledTrain->GetActorLocation().X, ControlledTrain->GetActorLocation().Y);
	EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(Loc2D);

	if (Zone == EZoningClassification::UrbanCenter)
	{
		TargetSpeedKmh = 40.0f;
	}
	else if (Zone == EZoningClassification::Nature)
	{
		TargetSpeedKmh = 120.0f;
	}
	else
	{
		TargetSpeedKmh = 80.0f;
	}

	// Mock 5km block signal check
	bool bBlockOccupied = false;
	TArray<AActor*> OtherTrains;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainPawn::StaticClass(), OtherTrains);
	FVector TrainLoc = ControlledTrain->GetActorLocation();
	FVector TrainForward = ControlledTrain->GetActorForwardVector();
	for (AActor* Actor : OtherTrains)
	{
		ATrainPawn* OtherTrain = Cast<ATrainPawn>(Actor);
		if (OtherTrain && OtherTrain != ControlledTrain)
		{
			// Issue 7: Only stop if the train is on the exact same track!
			if (OtherTrain->bOnParallelTrack == ControlledTrain->bOnParallelTrack)
			{
				FVector DirToOther = (OtherTrain->GetActorLocation() - TrainLoc).GetSafeNormal();
				float Dot = FVector::DotProduct(TrainForward, DirToOther);
				
				// Issue 15: If the train is physically in front of us, trigger block
				if (Dot > 0.5f) 
				{
					float Dist = FVector::Distance(TrainLoc, OtherTrain->GetActorLocation());
					if (Dist < 500000.0f) // 5km Block Check
					{
						bBlockOccupied = true;
						break;
					}
				}
			}
		}
	}

	ARailwaySignal* NextSignal = GetNextSignalAhead();
	
	if (bBlockOccupied || (NextSignal && NextSignal->CurrentState == ESignalState::Stop && FVector::Distance(TrainLoc, NextSignal->GetActorLocation()) < SafeFollowingDistance))
	{
		// Red Signal Ahead! Apply Brakes!
		ControlledTrain->SetThrottleNotch(0.0f);
		ControlledTrain->SetTargetBrakePressure(60.0f); // Apply heavy brakes (drop pipe from 90 to 60)
	}
	else
	{
		// Track is Clear! Maintain speed.
		float CurrentSpeedKmh = ControlledTrain->GetVelocity().Size() * 0.036f;
		
		if (CurrentSpeedKmh < TargetSpeedKmh)
		{
			ControlledTrain->SetTargetBrakePressure(90.0f); // Release brakes fully
			ControlledTrain->SetThrottleNotch(8.0f); // Full power
		}
		else
		{
			ControlledTrain->SetThrottleNotch(0.0f); // Coast to maintain speed
		}
	}

	float CurrentSpeedKmh = ControlledTrain->GetVelocity().Size() * 0.036f;
	if (CurrentSpeedKmh > TargetSpeedKmh * 1.5f) // Derail condition
	{
		if (!bIsDerailed)
		{
			bIsDerailed = true;
			GetWorldTimerManager().SetTimer(CleanupTimer, this, &AAITrainController::CleanupCrash, 600.0f, false);
		}
	}

	for (AActor* Actor : CachedCrossings)
	{
		ALevelCrossing* Crossing = Cast<ALevelCrossing>(Actor);
		if (Crossing && Crossing->GateMesh)
		{
			float DistToCrossing = FVector::Distance(TrainLoc, Crossing->GetActorLocation());
			if (DistToCrossing < 100000.0f) // 1 km
			{
				TArray<FOverlapResult> Overlaps;
				FCollisionShape Box = FCollisionShape::MakeBox(FVector(1000.0f, 1000.0f, 500.0f));
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(Crossing);
				GetWorld()->OverlapMultiByChannel(Overlaps, Crossing->GetActorLocation(), FQuat::Identity, ECC_Pawn, Box, QueryParams);
				bool bCarsPresent = false;
				for (const FOverlapResult& Overlap : Overlaps)
				{
					AActor* OverlapActor = Overlap.GetActor();
					if (OverlapActor && !OverlapActor->IsA(ATrainPawn::StaticClass()))
					{
						bCarsPresent = true;
						break;
					}
				}

				if (bCarsPresent)
				{
					FRotator CurrentRot = Crossing->GateMesh->GetRelativeRotation();
					FRotator TargetRot(0, 0, -90.0f);
					Crossing->GateMesh->SetRelativeRotation(FMath::RInterpConstantTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), 90.0f / 5.0f));
				}
			}
		}
	}
}

ARailwaySignal* AAITrainController::GetNextSignalAhead()
{
	// Use cached signal list — refreshed every 10s instead of scanning all world actors every tick
	ARailwaySignal* ClosestSignal = nullptr;
	float ClosestDist = 9999999.0f;
	FVector TrainLoc = ControlledTrain->GetActorLocation();
	FVector TrainForward = ControlledTrain->GetActorForwardVector();

	for (AActor* Actor : CachedSignals)
	{
		ARailwaySignal* Signal = Cast<ARailwaySignal>(Actor);
		if (Signal && IsValid(Signal))
		{
			FVector DirToSignal = (Signal->GetActorLocation() - TrainLoc).GetSafeNormal();
			float DotProduct = FVector::DotProduct(TrainForward, DirToSignal);

			// Only consider signals that are IN FRONT of the train
			if (DotProduct > 0.5f)
			{
				float Dist = FVector::Distance(TrainLoc, Signal->GetActorLocation());
				if (Dist < ClosestDist)
				{
					ClosestDist = Dist;
					ClosestSignal = Signal;
				}
			}
		}
	}

	return ClosestSignal;
}
