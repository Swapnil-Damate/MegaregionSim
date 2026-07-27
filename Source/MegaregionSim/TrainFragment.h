#pragma once

#include "CoreMinimal.h"
#include "TrainFragment.generated.h"

USTRUCT()
struct MEGAREGIONSIM_API FTrainFragment
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector Position;

	UPROPERTY()
	float Velocity;
	
	UPROPERTY()
	float Throttle;
};
