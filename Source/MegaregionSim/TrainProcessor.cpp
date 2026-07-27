#include "TrainProcessor.h"
#include "TrainFragment.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"

UTrainProcessor::UTrainProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UTrainProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTrainFragment>(EMassFragmentAccess::ReadWrite);
}

void UTrainProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, [&Context](FMassExecutionContext& ChunkContext)
	{
		const TArrayView<FTrainFragment> TrainList = ChunkContext.GetMutableFragmentView<FTrainFragment>();
		float DeltaTime = ChunkContext.GetDeltaTimeSeconds();

		for (int32 i = 0; i < ChunkContext.GetNumEntities(); ++i)
		{
			FTrainFragment& Train = TrainList[i];
			
			// Extremely basic mathematical AI simulation: 
			// If throttle is applied, accelerate. Update position linearly.
			float Acceleration = Train.Throttle * 10.0f; // 10 m/s^2 arbitrarily for virtual trains
			Train.Velocity += Acceleration * DeltaTime;
			
			// Move the train
			Train.Position.X += Train.Velocity * DeltaTime;
		}
	});
}
