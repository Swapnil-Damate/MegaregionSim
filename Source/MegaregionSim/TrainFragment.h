#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "TrainFragment.generated.h"

USTRUCT()
struct MEGAREGIONSIM_API FTrainFragment : public FMassFragment
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector Position;

	UPROPERTY()
	float Velocity;
	
	UPROPERTY()
	float Throttle;
};
