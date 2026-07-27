#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MegaregionWeatherSystem.generated.h"

UENUM(BlueprintType)
enum class EWeatherState : uint8
{
	Clear,
	Rain,
	Snow,
	HeavyFog
};

/**
 * Handles the dynamic time of day, Volumetric Clouds, Lumen lighting, 
 * and applies weather effects that directly degrade physics traction.
 */
UCLASS()
class MEGAREGIONSIM_API AMegaregionWeatherSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	AMegaregionWeatherSystem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EWeatherState CurrentWeather;

	// The speed at which the sun rotates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeScale;

	// Sets the weather state and immediately applies physics degradation (e.g. reduced wheel friction)
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherState(EWeatherState NewState);

private:
	// Find the Directional Light in the scene to rotate it
	class ADirectionalLight* SunLight;
	
	// Helper to find the sun
	void FindSunLight();
};
