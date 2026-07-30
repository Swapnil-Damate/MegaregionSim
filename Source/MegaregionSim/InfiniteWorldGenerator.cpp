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

static float GetGroundHeightForGenerator(UWorld* World, float X, float Y, float DefaultZ)
{
	if (!World) return DefaultZ;

	FVector TraceStart(X, Y, 50000.0f);
	FVector TraceEnd(X, Y, -20000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;

	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && (HitActor->GetName().Contains(TEXT("Track")) || HitActor->GetName().Contains(TEXT("Train")) || HitActor->GetName().Contains(TEXT("Car"))))
		{
			FCollisionQueryParams RefinedParams;
			RefinedParams.AddIgnoredActor(HitActor);
			if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, RefinedParams))
			{
				return HitResult.Location.Z;
			}
		}
		else
		{
			return HitResult.Location.Z;
		}
	}
	return DefaultZ;
}

void AInfiniteWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Find the player to track
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		TrackedPlayer = PC->GetPawn();
	}

	// Clean up any default template floor actor at the origin that blocks the track/landscape
	TArray<AActor*> FoundStaticMeshActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundStaticMeshActors);
	for (AActor* Actor : FoundStaticMeshActors)
	{
		if (Actor && (Actor->GetName().Contains(TEXT("Floor")) || Actor->GetName().Contains(TEXT("Plane")) || Actor->GetName().Contains(TEXT("Grid")) || FMath::Abs(Actor->GetActorLocation().Z) < 50.0f))
		{
			// Check if it's a huge flat floor
			FVector ActorScale = Actor->GetActorScale3D();
			if (ActorScale.X > 5.0f && ActorScale.Y > 5.0f && ActorScale.Z < 0.5f)
			{
				Actor->Destroy();
				UE_LOG(LogTemp, Warning, TEXT("Destroyed default template floor actor: %s"), *Actor->GetName());
			}
		}
	}

	// Initialize essential chunk math before anything generates
	if (ChunkLength <= 0.0f)
	{
		ChunkLength = 100000.0f; 
	}

	// Widen default lookahead to 5 chunks (5km)
	GenerationDistance = 5;

	// Query starting ground height
	LastSplineZ = GetGroundHeightForGenerator(GetWorld(), 0.0f, 0.0f, 0.0f);

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
		
		// Use raycasting to find the landscape height at this coordinate!
		float TargetZ = GetGroundHeightForGenerator(GetWorld(), LastSplineGenerationDistance, CurveY, 0.0f);
		
		// Smooth out height transitions to mimic grading (Max 1.5m vertical change per 50m segment - 3% grade)
		float MaxDeltaZ = 150.0f;
		float TargetDeltaZ = TargetZ - LastSplineZ;
		TargetDeltaZ = FMath::Clamp(TargetDeltaZ, -MaxDeltaZ, MaxDeltaZ);
		
		float CurveZ = LastSplineZ + TargetDeltaZ;
		LastSplineZ = CurveZ;
		
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
