#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LevelCrossing.generated.h"

UCLASS()
class MEGAREGIONSIM_API ALevelCrossing : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelCrossing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crossing")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crossing")
	UStaticMeshComponent* GateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crossing")
	UBoxComponent* WarningTrigger;

	UFUNCTION()
	void OnTrainEnterWarningZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTrainLeaveWarningZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	bool bIsTrainApproaching;
	float CurrentGateAngle;
	float TargetGateAngle;
	float TrafficSpawnTimer = 0.0f;
	
	UPROPERTY()
	TArray<UStaticMeshComponent*> TrafficCars;
};
