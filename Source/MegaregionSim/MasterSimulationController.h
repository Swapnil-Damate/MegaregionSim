#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/ExponentialHeightFog.h"
#include "MasterSimulationController.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class MEGAREGIONSIM_API AMasterSimulationController : public AActor
{
	GENERATED_BODY()
	
public:	
	AMasterSimulationController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Megaregion")
	void GenerateEnvironment();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PCG")
	UInstancedStaticMeshComponent* PineTreeISM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PCG")
	UInstancedStaticMeshComponent* SkyscraperISM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PCG")
	UInstancedStaticMeshComponent* GrassISM;

private:
	bool bWeatherApplied = false;
};
