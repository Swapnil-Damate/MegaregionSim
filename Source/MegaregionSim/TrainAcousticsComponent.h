#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrainAcousticsComponent.generated.h"


UCLASS( ClassGroup=(Megaregion), meta=(BlueprintSpawnableComponent) )
class MEGAREGIONSIM_API UTrainAcousticsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTrainAcousticsComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acoustics")
	bool bInTunnel;

	// In the final game this maps to a MetaSound, but here we drive proxy pitch logic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acoustics")
	float BaseEnginePitch;

	// Feeds into MetaSound Engine RPM
	UFUNCTION(BlueprintCallable, Category = "Acoustics")
	void UpdateEngineAcoustics(float CurrentRPM, float PhysicsVelocity);
	
	// Called by physics when the wheel passes a track joint
	UFUNCTION(BlueprintCallable, Category = "Acoustics")
	void TriggerTrackClack();

	UFUNCTION(BlueprintCallable, Category = "Acoustics")
	float CalculateDopplerPitch(float RelativeVelocity);

private:
	float DistanceSinceLastClack;
};
