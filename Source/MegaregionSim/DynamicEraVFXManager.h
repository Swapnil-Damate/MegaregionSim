#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicEraVFXManager.generated.h"

UENUM(BlueprintType)
enum class ETrainEraType : uint8
{
	Steam,
	Diesel,
	Electric,
	HighSpeed
};

/**
 * Manages procedural Niagara visual effects based on the train's era and physics state.
 */
UCLASS( ClassGroup=(Megaregion), meta=(BlueprintSpawnableComponent) )
class MEGAREGIONSIM_API UDynamicEraVFXManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDynamicEraVFXManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Era")
	ETrainEraType TrainType;

	// In the real system, this connects to a Niagara Particle System Component
	// For proxy, it logs the mathematical simulation of the VFX
	
	UFUNCTION(BlueprintCallable, Category = "VFX")
	void UpdateVFXState(float CurrentSpeed, float EngineLoad);
	
	UFUNCTION(BlueprintCallable, Category = "VFX")
	void TriggerPantographSpark();
};
