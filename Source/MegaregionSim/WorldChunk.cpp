#include "WorldChunk.h"
#include "Components/SplineComponent.h"
#include "MegaregionZoningGenerator.h"
#include "UObject/ConstructorHelpers.h"

AWorldChunk::AWorldChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	PineTreeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PineTreeISM"));
	PineTreeISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PineAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (PineAsset.Succeeded()) PineTreeISM->SetStaticMesh(PineAsset.Object);

	SkyscraperISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SkyscraperISM"));
	SkyscraperISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkyAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (SkyAsset.Succeeded()) SkyscraperISM->SetStaticMesh(SkyAsset.Object);
	
	TrackMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TrackMeshISM"));
	TrackMeshISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (TrackAsset.Succeeded()) TrackMeshISM->SetStaticMesh(TrackAsset.Object);
}

void AWorldChunk::InitializeChunk(AInfiniteWorldGenerator* Generator, USplineComponent* InSpline, float StartDistance, float EndDistance)
{
	GenerateTerrainInstances(InSpline, StartDistance, EndDistance);
	GenerateTrackSplineMeshes(InSpline, StartDistance, EndDistance);
}

void AWorldChunk::GenerateTerrainInstances(USplineComponent* Spline, float StartDist, float EndDist)
{
	// Scatter instances along this 1km spline segment
	float StepSize = 2500.0f; // Every 25 meters, spawn left/right
	for (float Dist = StartDist; Dist < EndDist; Dist += StepSize)
	{
		FVector SplineLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector SplineRight = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(SplineLoc.X, SplineLoc.Y));
		
		// Spawn left and right of the track
		for (float Offset = -10000.0f; Offset <= 10000.0f; Offset += 3000.0f)
		{
			if (FMath::Abs(Offset) < 1500.0f) continue; // Keep track clear

			FVector SpawnLoc = SplineLoc + (SplineRight * Offset);
			
			// Perlin noise for hills
			float NoiseScale = 0.00015f;
			float ZHeight = FMath::PerlinNoise2D(FVector2D(SpawnLoc.X * NoiseScale, SpawnLoc.Y * NoiseScale)) * 12000.0f;
			SpawnLoc.Z = ZHeight;

			FTransform InstanceTransform;
			InstanceTransform.SetLocation(SpawnLoc);
			InstanceTransform.SetScale3D(FVector(FMath::RandRange(0.8f, 1.5f)));
			InstanceTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));

			if (Zone == EZoningClassification::Nature || Zone == EZoningClassification::Village)
			{
				PineTreeISM->AddInstance(InstanceTransform);
			}
			else if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
			{
				// Buildings should probably spawn on flatter terrain or use different alignment, but this works for prototype
				SkyscraperISM->AddInstance(InstanceTransform);
			}
		}
	}
}

void AWorldChunk::GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist)
{
	// A simple approach using ISM for tracks (fast).
	float TrackMeshLength = 2500.0f; // 25 meters per mesh segment
	
	for (float Dist = StartDist; Dist < EndDist; Dist += TrackMeshLength)
	{
		FVector StartLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		
		FTransform TrackTransform1;
		TrackTransform1.SetLocation(StartLoc);
		TrackTransform1.SetRotation(StartRot.Quaternion());
		TrackTransform1.SetScale3D(FVector(1.0f, 1.0f, 1.0f)); 
		
		FTransform TrackTransform2;
		TrackTransform2.SetLocation(StartLoc + (RightVec * 500.0f)); // Double track offset 5 meters
		TrackTransform2.SetRotation(StartRot.Quaternion());
		TrackTransform2.SetScale3D(FVector(1.0f, 1.0f, 1.0f)); 
		
		TrackMeshISM->AddInstance(TrackTransform1);
		TrackMeshISM->AddInstance(TrackTransform2);
	}

	// --- Procedural Stations ---
	// Roughly every 10km (StartDist % 10000 == 0), spawn a station if we are in an urban zone
	if (FMath::Fmod(StartDist, 10000.0f) == 0.0f)
	{
		FVector Loc = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(Loc.X, Loc.Y));
		
		if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
		{
			// Spawn a basic Station Platform using Skyscraper mesh (Proxy)
			FTransform StationTransform;
			FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
			StationTransform.SetLocation(Loc - (RightVec * 1500.0f));
			StationTransform.SetScale3D(FVector(0.5f, 5.0f, 0.2f)); // Long, flat platform shape
			StationTransform.SetRotation(Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World).Quaternion());
			SkyscraperISM->AddInstance(StationTransform);
		}
	}
}
