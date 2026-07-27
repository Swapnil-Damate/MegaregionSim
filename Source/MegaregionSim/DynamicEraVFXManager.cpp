#include "DynamicEraVFXManager.h"

UDynamicEraVFXManager::UDynamicEraVFXManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	TrainType = ETrainEraType::Diesel;
}

void UDynamicEraVFXManager::BeginPlay()
{
	Super::BeginPlay();
}

void UDynamicEraVFXManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDynamicEraVFXManager::UpdateVFXState(float CurrentSpeed, float EngineLoad)
{
	// Scale the particle emission rate based on how hard the engine is working (EngineLoad)
	// and scale the particle velocity/drag based on the train's speed.
	
	if (TrainType == ETrainEraType::Steam)
	{
		// Map EngineLoad (0.0 to 1.0) to volumetric smoke thickness
		float SmokeDensity = EngineLoad * 100.0f;
		
		// Map CurrentSpeed to how fast the smoke trails backward
		float SmokeVelocity = CurrentSpeed * -1.5f;

		// Proxy Logging
		if (FMath::RandRange(0, 100) > 98)
		{
			UE_LOG(LogTemp, Log, TEXT("VFX Proxy [STEAM]: Niagara Smoke updated. Density: %f, Trail Velocity: %f"), SmokeDensity, SmokeVelocity);
		}
	}
	else if (TrainType == ETrainEraType::Diesel)
	{
		// Diesels mostly emit exhaust under heavy acceleration
		if (EngineLoad > 0.8f)
		{
			if (FMath::RandRange(0, 100) > 95)
			{
				UE_LOG(LogTemp, Log, TEXT("VFX Proxy [DIESEL]: Heavy acceleration! Niagara Exhaust bursting."));
			}
		}
	}
}

void UDynamicEraVFXManager::TriggerPantographSpark()
{
	if (TrainType == ETrainEraType::Electric || TrainType == ETrainEraType::HighSpeed)
	{
		UE_LOG(LogTemp, Warning, TEXT("VFX Proxy [ELECTRIC]: Pantograph disconnected from catenary wire! Triggering Niagara Spark Burst."));
	}
}
