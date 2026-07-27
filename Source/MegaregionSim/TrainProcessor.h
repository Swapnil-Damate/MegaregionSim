#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "TrainProcessor.generated.h"

UCLASS()
class MEGAREGIONSIM_API UTrainProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UTrainProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(class UMassEntitySubsystem& EntitySubsystem, class FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
