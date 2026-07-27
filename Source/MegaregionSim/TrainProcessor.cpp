#include "TrainProcessor.h"
#include "TrainFragment.h"
#include "MassExecutionContext.h"
#include "MassEntitySubsystem.h"
#include "MassEntityManager.h"

UTrainProcessor::UTrainProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
}

void UTrainProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTrainFragment>(EMassFragmentAccess::ReadWrite);
}

void UTrainProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&Context](FMassExecutionContext& ChunkContext)
	{
		const TArrayView<FTrainFragment> TrainList = ChunkContext.GetMutableFragmentView<FTrainFragment>();
		float DeltaTime = ChunkContext.GetDeltaTimeSeconds();

		for (int32 i = 0; i < ChunkContext.GetNumEntities(); ++i)
		{
			FTrainFragment& Train = TrainList[i];
			
			// Initialize dynamically if needed
			if (Train.Throttle == 0.0f)
			{
				Train.Position = FVector(FMath::RandRange(-250000.0f, 250000.0f), 0, 0);
				Train.Throttle = FMath::RandRange(0.1f, 1.0f);
			}

			// Extremely basic mathematical AI simulation: 
			// If throttle is applied, accelerate. Update position linearly.
			float Acceleration = Train.Throttle * 10.0f; // 10 m/s^2 arbitrarily for virtual trains
			Train.Velocity += Acceleration * DeltaTime;
			
			// Move the train
			Train.Position.X += Train.Velocity * DeltaTime;
		}
	});
}
