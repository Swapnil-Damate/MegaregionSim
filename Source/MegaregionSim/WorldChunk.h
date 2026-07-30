#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "WorldChunk.generated.h"

class USplineComponent;
class AInfiniteWorldGenerator;

UCLASS()
class MEGAREGIONSIM_API AWorldChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldChunk();

	void InitializeChunk(AInfiniteWorldGenerator* Generator, USplineComponent* InSpline, float StartDistance, float EndDistance);

protected:
	UPROPERTY()
	UInstancedStaticMeshComponent* PineTreeISM;
	
	UPROPERTY()
	UInstancedStaticMeshComponent* SkyscraperISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* TunnelISM;

	UPROPERTY()
	UInstancedStaticMeshComponent* TrackMeshISM;
	
	UPROPERTY()
	UInstancedStaticMeshComponent* SignalISM;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshes;

	void GenerateTerrainInstances(USplineComponent* Spline, float StartDist, float EndDist);
	void GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist);
};
