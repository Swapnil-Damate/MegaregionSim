#include "EmergencyDispatcher.h"
#include "Kismet/GameplayStatics.h"
#include "RailwaySignal.h"

AEmergencyDispatcher::AEmergencyDispatcher()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEmergencyDispatcher::BeginPlay()
{
	Super::BeginPlay();
}

void AEmergencyDispatcher::ReportDerailment(FVector DerailmentLocation)
{
	// 1. Freeze all signals in a 10km radius to Danger (RED)
	FreezeSignalsToRed(DerailmentLocation, 1000000.0f);
	
	// 2. Spawn Emergency Service vehicles (Fire/Ambulance) at nearest road splines
	// (Placeholder for future vehicle spawning logic)
}

void AEmergencyDispatcher::FreezeSignalsToRed(FVector Location, float Radius)
{
	TArray<AActor*> FoundSignals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARailwaySignal::StaticClass(), FoundSignals);
	
	for (AActor* SignalActor : FoundSignals)
	{
		if (FVector::Dist(SignalActor->GetActorLocation(), Location) <= Radius)
		{
			ARailwaySignal* Signal = Cast<ARailwaySignal>(SignalActor);
			if (Signal)
			{
				Signal->SetSignalState(ESignalState::Stop); // Force Red
			}
		}
	}
}
