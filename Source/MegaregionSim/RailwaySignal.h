#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "RailwaySignal.generated.h"

UENUM(BlueprintType)
enum class ESignalState : uint8
{
	Clear UMETA(DisplayName = "Clear"),
	Stop UMETA(DisplayName = "Stop")
};

UCLASS()
class MEGAREGIONSIM_API ARailwaySignal : public AActor
{
	GENERATED_BODY()
	
public:	
	ARailwaySignal();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal")
	UStaticMeshComponent* SignalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal")
	UBoxComponent* BlockTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal")
	ESignalState CurrentState;

	UFUNCTION()
	void OnTrainEnterBlock(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTrainLeaveBlock(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetSignalState(ESignalState NewState);
	
	// Called to visually update the signal (Material color change)
	void UpdateVisuals();
};
