#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PassengerAIController.generated.h"

UCLASS()
class MEGAREGIONSIM_API APassengerAIController : public AActor
{
	GENERATED_BODY()
	
public:	
	APassengerAIController();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SpawnPassengersAtStation(FVector StationLocation);

protected:
	virtual void BeginPlay() override;

private:
	float GlobalPassengerSatisfaction;
};
