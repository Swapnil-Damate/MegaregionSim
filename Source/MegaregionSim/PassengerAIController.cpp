#include "PassengerAIController.h"

APassengerAIController::APassengerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	GlobalPassengerSatisfaction = 100.0f;
}

void APassengerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APassengerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APassengerAIController::SpawnPassengersAtStation(FVector StationLocation)
{
	// In a full implementation, this uses a CrowdManager to spawn AI characters
	// navigating the platform using Recast NavMesh.
}
