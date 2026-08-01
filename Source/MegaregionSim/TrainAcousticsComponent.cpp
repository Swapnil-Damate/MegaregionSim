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
			float WindVolumeMultiplier = FMath::Clamp((SpeedKmH - 100.0f) * 0.02f, 0.0f, 1.0f);
			// Apply continuous wind volume multiplier. If we had an AudioComponent, we'd set its volume multiplier here.
		}

		// 4. Tunnel Reverb: If bInTunnel is true, muffle all sounds.
		if (bInTunnel)
		{
			// Applying Low Pass Filter for Tunnel Reverb
			// e.g. AudioComp->SetLowPassFilterEnabled(true);
		}
	}
}

void UTrainAcousticsComponent::UpdateEngineAcoustics(float CurrentRPM, float PhysicsVelocity)
{
	// Map RPM to an audio pitch multiplier (e.g. Idle = 1.0, Max RPM = 2.5)
	float TargetPitch = 1.0f + (CurrentRPM / 1000.0f); 
	
	// Doppler effect calculation (Phase 2): Calculate relative velocity to camera
	if (GetWorld() && GetWorld()->GetFirstPlayerController() && GetWorld()->GetFirstPlayerController()->PlayerCameraManager)
	{
		FVector CamLoc = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FVector ToCam = CamLoc - GetOwner()->GetActorLocation();
		ToCam.Normalize();
		
		float RelVel = FVector::DotProduct(GetOwner()->GetVelocity(), ToCam);
		TargetPitch *= CalculateDopplerPitch(RelVel);
	}
	
	BaseEnginePitch = TargetPitch;
}

void UTrainAcousticsComponent::TriggerTrackClack()
{
	// In production, this fires a OneShot MetaSound cue for track joint clatter
	// e.g. UGameplayStatics::PlaySoundAtLocation(this, TrackClackSound, GetOwner()->GetActorLocation());
}

float UTrainAcousticsComponent::CalculateDopplerPitch(float RelativeVelocity)
{
	float SpeedOfSound = 34300.0f; // cm/s
	// Pitch multiplier based on approach/recede
	float PitchMultiplier = SpeedOfSound / (SpeedOfSound + RelativeVelocity);
	return FMath::Clamp(PitchMultiplier, 0.1f, 10.0f);
}
