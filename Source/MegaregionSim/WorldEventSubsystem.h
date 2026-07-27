#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldEventSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDisasterType : uint8
{
	None,
	FlashFlood,
	Avalanche,
	Earthquake
};

UCLASS()
class MEGAREGIONSIM_API UWorldEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "World Events")
	void TriggerRandomDisaster();

	UFUNCTION(BlueprintCallable, Category = "World Events")
	EDisasterType GetActiveDisaster() const { return ActiveDisaster; }

	// Simulates the rusting/wear of infrastructure over long periods
	UFUNCTION(BlueprintCallable, Category = "World Events")
	float GetWorldAgingMultiplier() const { return WorldAgingMultiplier; }

private:
	EDisasterType ActiveDisaster;
	float WorldAgingMultiplier;
};
