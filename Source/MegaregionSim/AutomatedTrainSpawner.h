#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AutomatedTrainSpawner.generated.h"

UCLASS()
class MEGAREGIONSIM_API UAutomatedTrainSpawner : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	void SpawnVirtualTrains(int32 Count);
};
