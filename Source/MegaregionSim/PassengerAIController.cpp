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

void APassengerAIController::SpawnPassengersAtStation(FVector StationLocation, int32 StationDensityMultiplier)
{
	// Phase 14: Passenger Boarding Logistics & Economy Math
	// Calculate dynamic passenger throughput based on density multiplier
	int32 TotalBoarding = FMath::RandRange(10, 50) * StationDensityMultiplier;
	
	// Math: Each passenger pays a dynamic ticket price based on current demand
	float TicketPrice = FMath::RandRange(15.0f, 45.0f);
	int32 Revenue = FMath::RoundToInt(TotalBoarding * TicketPrice);
	
	if (UEconomySubsystem* Economy = GetWorld()->GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		Economy->AddFunds(Revenue);
		UE_LOG(LogTemp, Log, TEXT("Boarded %d passengers. Ticket Revenue: $%d"), TotalBoarding, Revenue);
	}
}
