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

	// Initialize essential chunk math before anything generates
	if (ChunkLength <= 0.0f)
	{
		ChunkLength = 100000.0f; 
	}

	// Widen default lookahead to 5 chunks (5km)
	GenerationDistance = 5;

	// Generate initial spline far ahead so all startup chunks have valid points
	GenerateSplineAhead(ChunkLength * (GenerationDistance + 2));
	
	// Force generate the first 6 chunks synchronously so the train NEVER falls into the void.
	// ManageChunks() only generates chunks near the player, but the player starts at X=20000
	// which is in chunk index 0. We pre-warm chunks 0..5 here.
	for (int32 i = 0; i <= GenerationDistance; i++)
	{
		if (!ActiveChunks.Contains(i))
		{
			FActorSpawnParameters SpawnParams;
			AWorldChunk* NewChunk = GetWorld()->SpawnActor<AWorldChunk>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (NewChunk)
			{
				float ChunkStart = i * ChunkLength;
				float ChunkEnd   = (i + 1) * ChunkLength;
				NewChunk->InitializeChunk(this, MainTrackSpline, ChunkStart, ChunkEnd);
				ActiveChunks.Add(i, NewChunk);
			}
		}
	}
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
		
		// Force spline to be perfectly dead-straight to prevent automatic tangent loops and crossing
		float CurveY = 0.0f; // No lateral noise!
		
		// Elevation math (perfectly flat for now to fix flying tracks)
		float CurveZ = 0.0f; // Force flat ground!
		
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
