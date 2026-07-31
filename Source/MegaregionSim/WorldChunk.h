#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "WorldChunk.generated.h"

class USplineComponent;
class AOpenWorldGraphGenerator;
class UMaterialInstanceDynamic;

UCLASS()
class MEGAREGIONSIM_API AWorldChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldChunk();

	UFUNCTION(BlueprintCallable, Category="World Chunk")
	void InitializeChunk(AOpenWorldGraphGenerator* Generator, USplineComponent* InSpline, float StartDistance, float EndDistance);

protected:
	UPROPERTY()
	UInstancedStaticMeshComponent* PineTreeISM;
	
	UPROPERTY()
	UInstancedStaticMeshComponent* BroadleafTreeISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* GrassISM;
	
	UPROPERTY()
	UInstancedStaticMeshComponent* SkyscraperISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* SmallHouseISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* FactoryISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* TunnelISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* BridgeISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* TrackMeshISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* TurnoutISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* CatenaryISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* MilepostISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* CrossingISM;
	
	UPROPERTY()
	UInstancedStaticMeshComponent* SignalISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* StationISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* RuralStationISM;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshes;

	void GenerateTerrainInstances(USplineComponent* Spline, float StartDist, float EndDist);
	void GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist);
};
