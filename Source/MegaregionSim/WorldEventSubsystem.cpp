#include "WorldEventSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"

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
