#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmergencyDispatcher.generated.h"

UCLASS()
class MEGAREGIONSIM_API AEmergencyDispatcher : public AActor
{
	GENERATED_BODY()
	
public:	
	AEmergencyDispatcher();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ReportDerailment(FVector DerailmentLocation);

protected:
	virtual void BeginPlay() override;

private:
	void FreezeSignalsToRed(FVector Location, float Radius);
};
