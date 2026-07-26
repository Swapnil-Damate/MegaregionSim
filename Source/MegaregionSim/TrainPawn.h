#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "TrainPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UTrainHUDWidget;
class UPhysicsConstraintComponent;
class USphereComponent;

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

	// Camera Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BrakeAction;

	void ThrottleInput(const FInputActionValue& Value);
	void BrakeInput(const FInputActionValue& Value);

	// UI Integration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UTrainHUDWidget> HUDWidgetClass;

	UPROPERTY()
	UTrainHUDWidget* HUDWidgetInstance;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics|Brakes")
	float TargetBrakePipePressure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics|Brakes")
	float BrakeExhaustRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics|Brakes")
	float BrakeChargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics|Brakes")
	float MaxBrakeForce;

	float TimeSinceLastHUDUpdate;

	// Input State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Input")
	float CurrentThrottleNotch;

	UFUNCTION(BlueprintCallable, Category = "Train Input")
	void SetThrottleNotch(float Notch);

	UFUNCTION(BlueprintCallable, Category = "Train Input")
	void SetTargetBrakePressure(float TargetPressure);

	// Physical Constraints acting as Knuckle Couplers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	UPhysicsConstraintComponent* RearCoupler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	USphereComponent* RearCouplerTrigger;

	UPROPERTY()
	AActor* RearAttachedCar;

	UFUNCTION()
	void OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
