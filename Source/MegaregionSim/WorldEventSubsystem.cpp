#include "WorldEventSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/ExponentialHeightFog.h"

void UWorldEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveDisaster = EDisasterType::None;
	WorldAgingMultiplier = 1.0f; // 1.0 = brand new, 0.0 = completely rusted and broken
}

void UWorldEventSubsystem::TriggerRandomDisaster()
{
	int32 RandomType = FMath::RandRange(1, 3);
	ActiveDisaster = static_cast<EDisasterType>(RandomType);
	
	// Accelerate aging when a disaster hits
	WorldAgingMultiplier -= 0.1f;
	if (WorldAgingMultiplier < 0.0f) WorldAgingMultiplier = 0.0f;
	
	// Spawning logic for disasters
	if (ActiveDisaster == EDisasterType::FlashFlood)
	{
		if (UWorld* World = GetWorld())
		{
			// Spawn a water plane for Flash Flood
			FVector SpawnLocation(0.0f, 0.0f, 1000.0f);
			FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
			AStaticMeshActor* WaterPlane = World->SpawnActor<AStaticMeshActor>(SpawnLocation, SpawnRotation);
			if (WaterPlane)
			{
				WaterPlane->SetActorScale3D(FVector(100.0f, 100.0f, 1.0f));
			}
		}
	}
	else if (ActiveDisaster == EDisasterType::Avalanche)
	{
		if (UWorld* World = GetWorld())
		{
			// Spawn physics spheres for Avalanche
			for (int i = 0; i < 10; ++i)
			{
				FVector SpawnLocation(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), 5000.0f);
				FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
				AStaticMeshActor* Boulder = World->SpawnActor<AStaticMeshActor>(SpawnLocation, SpawnRotation);
				if (Boulder)
				{
					Boulder->SetActorScale3D(FVector(5.0f));
				}
			}
		}
	}
}

void UWorldEventSubsystem::Tick(float DeltaTime)
{
	if (UWorld* World = GetWorld())
	{
		// 1. Randomly spawn an AExponentialHeightFog actor to simulate dense mornings
		if (FMath::RandRange(0, 10000) < 5)
		{
			World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass());
		}

		// 2. Dummy logic for Niagara particle systems to spawn rain/snow based on simple random chance
		if (FMath::RandRange(0, 10000) < 10)
		{
			// TODO: UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, RainOrSnowSystem, FVector::ZeroVector);
		}
		
		// 3. Phase 13: Realistic Day/Night Cycle
		for (TActorIterator<ADirectionalLight> It(World); It; ++It)
		{
			ADirectionalLight* Sun = *It;
			// A full 360 degree rotation at 0.005 degrees per tick equals approximately 1 realistic day cycle per 2 hours of gameplay
			Sun->AddActorLocalRotation(FRotator(0.005f, 0.0f, 0.0f));
		}
	}
}

TStatId UWorldEventSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldEventSubsystem, STATGROUP_Tickables);
}
