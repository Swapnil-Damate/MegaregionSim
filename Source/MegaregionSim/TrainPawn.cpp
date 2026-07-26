#include "TrainPawn.h"
#include "Math/UnrealMathUtility.h"

ATrainPawn::ATrainPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default massive weight (e.g. 10000 tons)
	MassInTons = 10000.0f;
	MaxTractiveEffort = 500000.0f; // Newtons
	
	// Default pressures in PSI (standard US freight brake setup)
	BrakePipePressure = 90.0f;
	MainReservoirPressure = 130.0f;
	BrakeCylinderPressure = 0.0f;
	
	CurrentThrottleNotch = 0.0f;
}

void ATrainPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATrainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Basic throttle logic placeholder
	if (CurrentThrottleNotch > 0.0f)
	{
		// Calculate linear force to be applied in the physics solver
		float AppliedForce = (CurrentThrottleNotch / 8.0f) * MaxTractiveEffort; 
	}
}

void ATrainPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATrainPawn::SetThrottleNotch(float Notch)
{
	// Locomotives typically have 8 throttle notches
	CurrentThrottleNotch = FMath::Clamp(Notch, 0.0f, 8.0f);
}
