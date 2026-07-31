#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RailwaySignal.h"
#include "Misc/DateTime.h"
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

	UPROPERTY(BlueprintReadWrite, Category = "Schedule")
	FString DestinationCity;

	UPROPERTY(BlueprintReadWrite, Category = "Schedule")
	FDateTime ScheduleTime;

private:
	UPROPERTY()
	ATrainPawn* ControlledTrain;
	
	// AI Logic
	void ScanForSignals();
	ARailwaySignal* GetNextSignalAhead();

	float TargetSpeedKmh;
	float SafeFollowingDistance;

	// Cached signal list to avoid GetAllActorsOfClass every tick
	TArray<AActor*> CachedSignals;
	FTimerHandle SignalCacheTimer;
	void RefreshSignalCache();

	TArray<AActor*> CachedCrossings;

	bool bIsDerailed = false;
	FTimerHandle CleanupTimer;
	void CleanupCrash();
};
