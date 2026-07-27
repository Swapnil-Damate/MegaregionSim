#include "MetroTrain.h"

AMetroTrain::AMetroTrain()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bAreDoorsOpen = false;
	PassengerSatisfaction = 100.0f;
	
	// Metro trains have extreme acceleration but lower top speed
	MassInTons = 5.0f;
	MaxTractiveEffort = 6000000.0f; 
}

void AMetroTrain::BeginPlay()
{
	Super::BeginPlay();
}

void AMetroTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	HandleStationStop(DeltaTime);
}

void AMetroTrain::HandleStationStop(float DeltaTime)
{
	float SpeedMetersPerSecond = GetVelocity().Size() * 0.01f;
	
	// If stopped at a station (speed is 0 and brakes are applied)
	if (SpeedMetersPerSecond < 0.1f && BrakeCylinderPressure > 20.0f)
	{
		bAreDoorsOpen = true;
		// Passengers board...
	}
	else
	{
		bAreDoorsOpen = false;
		
		// Penalize satisfaction for extreme G-forces (rapid braking)
		if (BrakeCylinderPressure > 50.0f && SpeedMetersPerSecond > 10.0f)
		{
			PassengerSatisfaction -= 1.0f * DeltaTime;
			if (PassengerSatisfaction < 0.0f) PassengerSatisfaction = 0.0f;
		}
	}
}
