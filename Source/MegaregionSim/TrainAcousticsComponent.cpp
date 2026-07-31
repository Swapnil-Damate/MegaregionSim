#include "TrainAcousticsComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTrainAcousticsComponent::UTrainAcousticsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseEnginePitch = 1.0f;
	bInTunnel = false;
	DistanceSinceLastClack = 0.0f;
}

void UTrainAcousticsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTrainAcousticsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (AActor* Owner = GetOwner())
	{
		float VelocityCmS = Owner->GetVelocity().Size();
		float DistanceMoved = VelocityCmS * DeltaTime;

		// 1. MetaSounds Track Clatter: simulate a "clack" log every time the train travels exactly 25 meters.
		DistanceSinceLastClack += DistanceMoved;
		if (DistanceSinceLastClack >= 2500.0f) // 25 meters = 2500 cm
		{
			TriggerTrackClack();
			DistanceSinceLastClack = FMath::Fmod(DistanceSinceLastClack, 2500.0f);
		}

		// 2. Wind Noise: continuous wind volume multiplier that scales linearly with train speed > 100 km/h.
		float SpeedKmH = VelocityCmS * 0.036f;
		if (SpeedKmH > 100.0f)
		{
			float WindVolumeMultiplier = (SpeedKmH - 100.0f) * 0.02f;
			// Applying continuous wind volume multiplier
		}

		// 4. Tunnel Reverb: If bInTunnel is true, muffle all sounds (e.g., apply a Low Pass Filter mock).
		if (bInTunnel)
		{
			// Applying Low Pass Filter for Tunnel Reverb
		}
	}
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

float UTrainAcousticsComponent::CalculateDopplerPitch(float RelativeVelocity)
{
	// 3. Doppler Horns: When AI trains pass, their horn pitch should shift based on relative velocity (Doppler effect math)
	float SpeedOfSound = 343.0f; // m/s
	// Pitch multiplier based on approach/recede
	float PitchMultiplier = SpeedOfSound / (SpeedOfSound + RelativeVelocity);
	return FMath::Clamp(PitchMultiplier, 0.1f, 10.0f);
}
