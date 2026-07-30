#include "AITrainController.h"
#include "TrainPawn.h"
#include "Kismet/GameplayStatics.h"
#include "MegaregionZoningGenerator.h"

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

	ARailwaySignal* NextSignal = GetNextSignalAhead();
	
	if (NextSignal)
	{
		float DistanceToSignal = FVector::Distance(ControlledTrain->GetActorLocation(), NextSignal->GetActorLocation());
		
		if (NextSignal->CurrentState == ESignalState::Stop && DistanceToSignal < SafeFollowingDistance)
		{
			// Red Signal Ahead! Apply Brakes!
			ControlledTrain->SetThrottleNotch(0.0f);
			ControlledTrain->SetTargetBrakePressure(60.0f); // Apply heavy brakes (drop pipe from 90 to 60)
			return;
		}
	}

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
