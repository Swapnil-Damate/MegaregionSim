#pragma once

#include "CoreMinimal.h"
#include "TrainPawn.h"
#include "SteamLocomotive.generated.h"

UCLASS()
class MEGAREGIONSIM_API ASteamLocomotive : public ATrainPawn
{
	GENERATED_BODY()
	
public:
	ASteamLocomotive();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	// Steam specific physics variables
	float BoilerPressure;
	float FireboxTemperature;
	float WaterLevel;
	float CoalLevel;
	
	// Cutoff valve (reverser) simulating steam expansion efficiency
	float CutoffPercentage;

	void CalculateSteamTraction(float DeltaTime);
};
