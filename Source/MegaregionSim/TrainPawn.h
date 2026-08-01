#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrainAcousticsComponent.h"
#include "DynamicEraVFXManager.h"
#include "InputActionValue.h"
#include "TrainPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UPhysicsConstraintComponent;
class USphereComponent;
class UWidgetComponent;
class USpotLightComponent;
class UNiagaraComponent;
class USplineComponent;
class AMegaregionGameMode;

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

	// ── Components ─────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Components")
	UTrainAcousticsComponent* AcousticsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Components")
	UDynamicEraVFXManager* EraVFXManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	USpotLightComponent* Headlight;

	void ToggleHeadlight();
	void ToggleWipers();

	// ── Enhanced Input ─────────────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BrakeAction;

	void ThrottleInput(const FInputActionValue& Value);
	void BrakeInput(const FInputActionValue& Value);

	// ── Train Physics Variables ────────────────────────────────────────────────
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics|Brakes")
	float DynamicBrakeEffort;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics|Brakes")
	float ContinuousBrakeTime;

	float TimeSinceLastHUDUpdate;

	// ── Throttle & speed state ─────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Input")
	float CurrentThrottleNotch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Input")
	float CurrentThrust;

	/** Current kinematic speed in m/s — drives both movement and HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float CurrentSpeedMs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float TargetThrottleNotch = 0.0f; // For 3-second turbo spool delay

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float BrakeTemperature = 20.0f; // Ambient temp

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	bool bWipersActive = false;

	// Current distance along the infinite track spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float CurrentDistanceAlongSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics")
	bool bAutoRouteEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	bool bIsWheelSlipping = false;

	int32 CouplerSlackClanksRemaining = 0;
	float CouplerSlackTimer = 0.0f;
	bool bWasStopped = true;

	// Reference to the active track spline component
	UPROPERTY(BlueprintReadOnly, Category = "Train Physics")
	USplineComponent* MainTrackSplineRef;

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

	UPROPERTY()
	APawn* CinematicDroneCam = nullptr;

	void ToggleCinematicCamera();

	// ── Kinematic consist — cars are positioned each Tick ─────────────────────
	UPROPERTY()
	TArray<AActor*> ConsistCars;

	// ── Physical Rear Coupler (overlap trigger) ────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	UPhysicsConstraintComponent* RearCoupler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	USphereComponent* RearCouplerTrigger;

	UPROPERTY()
	AActor* RearAttachedCar;

	UFUNCTION()
	void OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void LogPhysicsState();

	void PlayHorn();
	void SwitchTrack();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwitchTrack();
	void DerailTrain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* CatenarySparks;

	/** True when train is on the parallel track (toggled via turnout) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	bool bOnParallelTrack = false;

	/** Lateral offset for parallel track in cm (3500 = 35m) */
	float ParallelTrackOffset = 3500.0f;
};
