#pragma once

#include "CoreMinimal.h"
#include "TrainPawn.h"
#include "HSRTrain.generated.h"

UCLASS()
class MEGAREGIONSIM_API AHSRTrain : public ATrainPawn
{
	GENERATED_BODY()
	
public:
	AHSRTrain();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	// Aerodynamic physics profile for High-Speed Rail (e.g. 350 km/h)
	float AerodynamicDragCoefficient;
	float ActivePantographTension;

	void CalculateHighSpeedAerodynamics(float DeltaTime);
};
