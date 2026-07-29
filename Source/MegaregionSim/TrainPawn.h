#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "TrainAcousticsComponent.h"
#include "DynamicEraVFXManager.h"
#include "InputActionValue.h"
#include "TrainPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UTrainHUDWidget;
class UPhysicsConstraintComponent;
class USphereComponent;
class UWidgetComponent;
class USpotLightComponent;
class UNiagaraComponent;

UCLASS(Blueprintable)
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

	// UI Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HUDWidgetComponent;

	// Acoustics Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acoustics", meta = (AllowPrivateAccess = "true"))
	UTrainAcousticsComponent* AcousticsComponent;

	// VFX Manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	UDynamicEraVFXManager* EraVFXManager;

	// Camera Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USpotLightComponent* Headlight;

	void ToggleHeadlight();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Input")
	float CurrentThrust;

	UFUNCTION(BlueprintCallable, Category = "Train Input")
	void SetThrottleNotch(float Notch);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetThrottleNotch(float Notch);

	UFUNCTION(BlueprintCallable, Category = "Train Input")
	void SetTargetBrakePressure(float TargetPressure);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetTargetBrakePressure(float TargetPressure);

	void SpawnConsist();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Input")
	UInputAction* SwitchTrainAction;
	
	void SwitchTrainInput(const FInputActionValue& Value);

	// Physical Constraints acting as Knuckle Couplers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	UPhysicsConstraintComponent* RearCoupler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	USphereComponent* RearCouplerTrigger;

	UPROPERTY()
	AActor* RearAttachedCar;

	UFUNCTION()
	void OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void LogPhysicsState();

	// Phase 13 Features
	void PlayHorn();
	void SwitchTrack();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwitchTrack();
	void DerailTrain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* CatenarySparks;
};
