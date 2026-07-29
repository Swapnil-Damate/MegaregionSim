#include "InfiniteWorldGenerator.h"
#include "WorldChunk.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

AInfiniteWorldGenerator::AInfiniteWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	MainTrackSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MainTrackSpline"));
	MainTrackSpline->SetupAttachment(RootComponent);
	
	// Clear default points
	MainTrackSpline->ClearSplinePoints();
	MainTrackSpline->AddSplinePoint(FVector(0,0,0), ESplineCoordinateSpace::World);
	MainTrackSpline->AddSplinePoint(FVector(1000,0,0), ESplineCoordinateSpace::World); // Initial direction
}

void AInfiniteWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Find the player to track
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		TrackedPlayer = PC->GetPawn();
	}

	// Generate initial spline points
	GenerateSplineAhead(ChunkLength * GenerationDistance);
}

void AInfiniteWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TrackedPlayer)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			TrackedPlayer = PC->GetPawn();
		}
		return;
	}

	ManageChunks();
}

void AInfiniteWorldGenerator::GenerateSplineAhead(float TargetDistance)
{
	while (LastSplineGenerationDistance < TargetDistance)
	{
		LastSplineGenerationDistance += 5000.0f; // Add point every 50m
		
		// Procedural curve math (gentle curves to prevent train derailment)
		float NoiseX = FMath::PerlinNoise1D(LastSplineGenerationDistance * 0.00001f);
		float CurveY = NoiseX * 100000.0f; // Up to 1km deviation laterally
		
		// Elevation math (gradual slopes)
		float NoiseZ = FMath::PerlinNoise1D((LastSplineGenerationDistance + 50000) * 0.000005f);
		float CurveZ = NoiseZ * 15000.0f; 
		
		FVector NewPoint(LastSplineGenerationDistance, CurveY, CurveZ);
		MainTrackSpline->AddSplinePoint(NewPoint, ESplineCoordinateSpace::World, true);
	}
}

void AInfiniteWorldGenerator::ManageChunks()
{
	if (!TrackedPlayer) return;
	
	if (ChunkLength <= 0.0f)
	{
		ChunkLength = 100000.0f; // Prevent Division By Zero
	}

	// Find nearest spline distance to player
	float PlayerInputKey = MainTrackSpline->FindInputKeyClosestToWorldLocation(TrackedPlayer->GetActorLocation());
	float PlayerDistanceAlongSpline = MainTrackSpline->GetDistanceAlongSplineAtSplineInputKey(PlayerInputKey);

	// Calculate which chunk index the player is currently in
	int32 CurrentChunkIndex = FMath::FloorToInt(PlayerDistanceAlongSpline / ChunkLength);

	// Generate new Spline points if we are getting close to the end
	float NeededSplineLength = (CurrentChunkIndex + GenerationDistance + 2) * ChunkLength;
	if (LastSplineGenerationDistance < NeededSplineLength)
	{
		GenerateSplineAhead(NeededSplineLength);
	}

	// Spawn Chunks Ahead
	for (int32 i = CurrentChunkIndex; i <= CurrentChunkIndex + GenerationDistance; i++)
	{
		if (!ActiveChunks.Contains(i) && i >= 0)
		{
			FActorSpawnParameters SpawnParams;
			AWorldChunk* NewChunk = GetWorld()->SpawnActor<AWorldChunk>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (NewChunk)
			{
				float ChunkStartDist = i * ChunkLength;
				float ChunkEndDist = (i + 1) * ChunkLength;
				NewChunk->InitializeChunk(this, MainTrackSpline, ChunkStartDist, ChunkEndDist);
				ActiveChunks.Add(i, NewChunk);
			}
		}
	}

	// Destroy Chunks Behind
	TArray<int32> KeysToRemove;
	for (auto& Elem : ActiveChunks)
	{
		int32 ChunkIndex = Elem.Key;
		if (ChunkIndex < CurrentChunkIndex - CleanupDistance)
		{
			AWorldChunk* ChunkToDestroy = Elem.Value;
			if (ChunkToDestroy)
			{
				ChunkToDestroy->Destroy();
			}
			KeysToRemove.Add(ChunkIndex);
		}
	}

	for (int32 Key : KeysToRemove)
	{
		ActiveChunks.Remove(Key);
	}
}
