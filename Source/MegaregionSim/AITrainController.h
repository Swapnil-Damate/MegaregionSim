#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RailwaySignal.h"
#include "AITrainController.generated.h"

class ATrainPawn;

UCLASS()
class MEGAREGIONSIM_API AAITrainController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAITrainController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	ATrainPawn* ControlledTrain;
	
	// AI Logic
	void ScanForSignals();
	ARailwaySignal* GetNextSignalAhead();

	float TargetSpeedKmh;
	float SafeFollowingDistance;
};
