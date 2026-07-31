#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "OpenWorldGraphGenerator.generated.h"

USTRUCT(BlueprintType)
struct FCityNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Population;
	
	// Array of connected node indices
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> ConnectedNodes;
};

UCLASS()
class MEGAREGIONSIM_API AOpenWorldGraphGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AOpenWorldGraphGenerator();

	virtual void Tick(float DeltaTime) override;

	// Procedural Math Functions
	UFUNCTION(BlueprintCallable, Category = "Megaregion|PCG")
	static float GetProceduralTerrainZ(float X, float Y);

	UFUNCTION(BlueprintCallable, Category = "Megaregion|PCG")
	static float GetVoronoiNoise(float X, float Y);

	// The 4 main tracks (2 Express, 2 Freight)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackNetwork")
	USplineComponent* ExpressTrackForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackNetwork")
	USplineComponent* ExpressTrackReverse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackNetwork")
	USplineComponent* FreightTrackForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackNetwork")
	USplineComponent* FreightTrackReverse;

	// Graph Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackNetwork")
	TArray<FCityNode> CityGraph;

	UFUNCTION(BlueprintCallable, Category = "Megaregion|PCG")
	void GenerateNextChunk(FVector PlayerLoc);

protected:
	virtual void BeginPlay() override;

private:
	void GenerateWorldGraph();
	void ConnectCitiesAStar(int32 NodeA, int32 NodeB);
	void SpawnCityStation(const FCityNode& Node);
	
	FString GenerateRandomCityName();
};
