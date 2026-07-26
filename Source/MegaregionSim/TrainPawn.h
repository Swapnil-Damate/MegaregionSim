#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrainPawn.generated.h"

UCLASS()
class MEGAREGIONSIM_API ATrainPawn : public APawn
{
	GENERATED_BODY()

public:
	ATrainPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Train Physics Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics")
	float MassInTons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics")
	float MaxTractiveEffort;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics|Brakes")
	float BrakePipePressure;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics|Brakes")
	float MainReservoirPressure;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics|Brakes")
	float BrakeCylinderPressure;

	// Input State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Input")
	float CurrentThrottleNotch;

	UFUNCTION(BlueprintCallable, Category = "Train Input")
	void SetThrottleNotch(float Notch);
};
