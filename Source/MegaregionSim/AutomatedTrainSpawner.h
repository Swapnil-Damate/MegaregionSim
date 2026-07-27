#pragma once

#include "CoreMinimal.h"
#include "Subsystems/TickableWorldSubsystem.h"
#include "TrainFragment.h"
#include "AutomatedTrainSpawner.generated.h"

UCLASS()
class MEGAREGIONSIM_API UAutomatedTrainSpawner : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); }

private:
	void SpawnVirtualTrains(int32 Count);

	TArray<FTrainFragment> VirtualTrains;
};
