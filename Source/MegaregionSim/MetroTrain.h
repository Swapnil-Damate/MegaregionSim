#pragma once

#include "CoreMinimal.h"
#include "TrainPawn.h"
#include "MetroTrain.generated.h"

UCLASS()
class MEGAREGIONSIM_API AMetroTrain : public ATrainPawn
{
	GENERATED_BODY()
	
public:
	AMetroTrain();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	// Urban Metro dynamics
	bool bAreDoorsOpen;
	float PassengerSatisfaction;
	
	void HandleStationStop(float DeltaTime);
};
