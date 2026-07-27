#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainCar.generated.h"

class UBoxComponent;
class UPhysicsConstraintComponent;
class USphereComponent;

UCLASS()
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Physics")
	float MassInTons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float BrakePipePressure; // Current pressure (0 to 90 PSI)

	// Physics offset for sloshing
	FVector LastVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train Physics")
	float BrakeCylinderPressure;

	// Linked Train Cars
	UPROPERTY()
	AActor* FrontAttachedCar;

	UPROPERTY()
	AActor* RearAttachedCar;

	// Overlap event for automatic coupling
	UFUNCTION()
	void OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
