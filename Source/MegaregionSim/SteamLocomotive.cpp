#include "SteamLocomotive.h"
#include "Math/UnrealMathUtility.h"

ASteamLocomotive::ASteamLocomotive()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoilerPressure = 200.0f; // PSI
	FireboxTemperature = 1500.0f; // Celsius
	WaterLevel = 100.0f; // Percentage
	CoalLevel = 100.0f; // Percentage
	CutoffPercentage = 75.0f; // Reverse gear starting cutoff
	
	// Heavy and slow, powerful pull
	MassInTons = 20.0f;
	MaxTractiveEffort = 3000000.0f;
}

void ASteamLocomotive::BeginPlay()
{
	Super::BeginPlay();
}

void ASteamLocomotive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateSteamTraction(DeltaTime);
}

void ASteamLocomotive::CalculateSteamTraction(float DeltaTime)
{
	// A placeholder for the complex thermodynamic calculations of a steam engine.
	// As throttle is applied, steam is consumed, dropping BoilerPressure.
	// Firebox must be stoked to recover pressure.
	
	if (CurrentThrottleNotch > 0.0f)
	{
		BoilerPressure -= (CurrentThrottleNotch * 0.1f) * DeltaTime;
		if (BoilerPressure < 0.0f) BoilerPressure = 0.0f;
	}
	
	// Recover pressure if fire is hot
	if (FireboxTemperature > 1000.0f && WaterLevel > 10.0f)
	{
		BoilerPressure += 0.05f * DeltaTime;
		if (BoilerPressure > 250.0f) BoilerPressure = 250.0f; // Safety valve pops
	}
}
