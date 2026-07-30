#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainCar.generated.h"

class UBoxComponent;
class UPhysicsConstraintComponent;
class USphereComponent;

UCLASS(Blueprintable)
class MEGAREGIONSIM_API ATrainCar : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrainCar();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// The physical body of the car
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	UBoxComponent* CarBody;

	// Physical Constraints acting as Knuckle Couplers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	UPhysicsConstraintComponent* FrontCoupler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	UPhysicsConstraintComponent* RearCoupler;

	// Overlap spheres to detect when to auto-couple
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	USphereComponent* FrontCouplerTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coupler")
	USphereComponent* RearCouplerTrigger;

	// Physics Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	float MassInTons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Fluid Dynamics")
	bool bIsLiquidCargo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics|Fluid Dynamics")
	FVector CurrentCenterOfMassOffset;
	
	// Track velocity to calculate acceleration for sloshing
	FVector LastVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float BrakePipePressure; // Current pressure (0 to 90 PSI)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float BrakeCylinderPressure;

	// Timer to throttle per-tick disk logging (prevents 480 file writes/sec with 8 cars)
	float PhysicsLogTimer = 0.0f;

	// Linked Train Cars
	UPROPERTY()
	AActor* FrontAttachedCar;

	UPROPERTY()
	AActor* RearAttachedCar;

	// Overlap event for automatic coupling
	UFUNCTION()
	void OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCarHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void LogPhysicsState();
};
