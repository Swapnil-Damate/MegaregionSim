#include "WorldEventSubsystem.h"
#include "Math/UnrealMathUtility.h"

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
	
	// In a full implementation, this would broadcast an event to the EmergencyDispatcher
	// and deform the voxel terrain mesh to show the landslide or flood.
}
