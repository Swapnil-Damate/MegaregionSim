#include "TrainAcousticsComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTrainAcousticsComponent::UTrainAcousticsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseEnginePitch = 1.0f;
}

void UTrainAcousticsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTrainAcousticsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTrainAcousticsComponent::UpdateEngineAcoustics(float CurrentRPM, float PhysicsVelocity)
{
	// Map RPM to an audio pitch multiplier (e.g. Idle = 1.0, Max RPM = 2.5)
	float TargetPitch = 1.0f + (CurrentRPM / 1000.0f); // Example formula
	
	// Doppler effect calculation could happen here based on camera distance
	
	// In the real system, this pushes the TargetPitch variable to the MetaSound parameter
	// Proxy implementation: just log it out periodically
	if (FMath::RandRange(0, 100) > 98)
	{
		UE_LOG(LogTemp, Log, TEXT("Acoustics Proxy: MetaSound Pitch updated to %f based on RPM %f"), TargetPitch, CurrentRPM);
	}
}

void UTrainAcousticsComponent::TriggerTrackClack()
{
	// Called by wheel collision logic exactly when hitting a procedural PCG track joint
	UE_LOG(LogTemp, Log, TEXT("Acoustics Proxy: Triggering MetaSound [Track_Clack.wav] node."));
}
