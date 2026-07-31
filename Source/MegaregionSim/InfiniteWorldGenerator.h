#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "InfiniteWorldGenerator.generated.h"

class AWorldChunk;

UCLASS()
class MEGAREGIONSIM_API AInfiniteWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteWorldGenerator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	static float GetProceduralTerrainZ(float X, float Y);

	// The massive infinite spine track
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Infinite World")
	USplineComponent* MainTrackSpline;

	// Config
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infinite World")
	float ChunkLength = 100000.0f; // 1km chunks
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infinite World")
	int32 GenerationDistance = 3; // Number of chunks ahead to spawn
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infinite World")
	int32 CleanupDistance = 2; // Number of chunks behind to destroy

private:
	// Tracks the player to know when to spawn/destroy chunks
	UPROPERTY()
	AActor* TrackedPlayer;

	// List of active chunks
	UPROPERTY()
	TMap<int32, AWorldChunk*> ActiveChunks;
	
	// Track spline progression
	float LastSplineGenerationDistance = 0.0f;
	float LastSplineZ = 0.0f;
	
	// Internal functions
	void GenerateSplineAhead(float Distance);
	void ManageChunks();
};
