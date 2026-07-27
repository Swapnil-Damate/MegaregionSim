#include "HSRTrain.h"
#include "Components/PrimitiveComponent.h"

AHSRTrain::AHSRTrain()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Sleek aerodynamics profile
	AerodynamicDragCoefficient = 0.25f; 
	ActivePantographTension = 100.0f;
	
	// High Speed Rails are lighter and have massive electric motors
	MassInTons = 4.0f; 
	MaxTractiveEffort = 8000000.0f; 
}

void AHSRTrain::BeginPlay()
{
	Super::BeginPlay();
}

void AHSRTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateHighSpeedAerodynamics(DeltaTime);
}

void AHSRTrain::CalculateHighSpeedAerodynamics(float DeltaTime)
{
	// At 300+ km/h, aerodynamic drag becomes the dominant force resisting movement.
	float SpeedMetersPerSecond = GetVelocity().Size() * 0.01f;
	float DragForce = AerodynamicDragCoefficient * (SpeedMetersPerSecond * SpeedMetersPerSecond);
	
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
	{
		FVector ForwardVector = GetActorForwardVector();
		// Apply drag opposite to velocity
		PrimComp->AddForce(-ForwardVector * DragForce);
	}
}
