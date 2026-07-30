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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PineAsset(TEXT("StaticMesh'/Game/FinalAssets/The_Pine_Tree.The_Pine_Tree'"));
	if (PineAsset.Succeeded()) PineTreeISM->SetStaticMesh(PineAsset.Object);

	GrassISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GrassISM"));
	GrassISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GrassAsset(TEXT("StaticMesh'/Game/FinalAssets/Grass_patch.Grass_patch'"));
	if (GrassAsset.Succeeded()) GrassISM->SetStaticMesh(GrassAsset.Object);

	SkyscraperISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SkyscraperISM"));
	SkyscraperISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkyAsset(TEXT("StaticMesh'/Game/FinalAssets/Skyscraper.Skyscraper'"));
	if (SkyAsset.Succeeded()) SkyscraperISM->SetStaticMesh(SkyAsset.Object);
	
	TrackMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TrackMeshISM"));
	TrackMeshISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackAsset(TEXT("StaticMesh'/Game/FinalAssets/Track_Mesh.Track_Mesh'"));
	if (TrackAsset.Succeeded()) TrackMeshISM->SetStaticMesh(TrackAsset.Object);

	TunnelISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TunnelISM"));
	TunnelISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TunnelAsset(TEXT("StaticMesh'/Game/FinalAssets/Tunnel_Mesh.Tunnel_Mesh'"));
	if (TunnelAsset.Succeeded()) TunnelISM->SetStaticMesh(TunnelAsset.Object);

	SignalISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SignalISM"));
	SignalISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SignalAsset(TEXT("StaticMesh'/Game/FinalAssets/Modern_LED_Signal.Modern_LED_Signal'"));
	if (SignalAsset.Succeeded()) SignalISM->SetStaticMesh(SignalAsset.Object);
}

void AWorldChunk::InitializeChunk(AInfiniteWorldGenerator* Generator, USplineComponent* InSpline, float StartDistance, float EndDistance)
{
	GenerateTerrainInstances(InSpline, StartDistance, EndDistance);
	GenerateTrackSplineMeshes(InSpline, StartDistance, EndDistance);
}

void AWorldChunk::GenerateTerrainInstances(USplineComponent* Spline, float StartDist, float EndDist)
{
	float StepSize = 2500.0f; // Every 25 meters
	for (float Dist = StartDist; Dist < EndDist; Dist += StepSize)
	{
		FVector SplineLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector SplineRight = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(SplineLoc.X, SplineLoc.Y));
		
		float NoiseScale = 0.00015f;
		float TrackMountainZ = FMath::PerlinNoise2D(FVector2D(SplineLoc.X * NoiseScale, SplineLoc.Y * NoiseScale)) * 12000.0f;
		bool bIsTunnel = false;
		if (TrackMountainZ > SplineLoc.Z + 1500.0f)
		{
			bIsTunnel = true;
			// Tunnel mesh scale is corrupt in the asset, disabling procedural spawn to fix visuals
			// FTransform TunnelTransform;
			// TunnelTransform.SetLocation(SplineLoc);
			// TunnelTransform.SetRotation(Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World).Quaternion());
			// TunnelTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
			// TunnelISM->AddInstance(TunnelTransform);
		}
		
		for (float Offset = -10000.0f; Offset <= 10000.0f; Offset += 3000.0f)
		{
			if (FMath::Abs(Offset) < 2500.0f) continue; // Wider clearance for track
			if (bIsTunnel) continue; // Skip trees around tunnel entrances to avoid clipping

			FVector SpawnLoc = SplineLoc + (SplineRight * Offset);
			SpawnLoc.Z = SplineLoc.Z - 200.0f; // Sink into the ground to prevent floating

				FTransform InstanceTransform;
				InstanceTransform.SetLocation(SpawnLoc);
				
				if (Zone == EZoningClassification::Nature || Zone == EZoningClassification::Village)
				{
					InstanceTransform.SetScale3D(FVector(FMath::RandRange(0.8f, 1.5f)));
					InstanceTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
					PineTreeISM->AddInstance(InstanceTransform);
					
					// Scatter Grass nearby
					for (int i = 0; i < 3; i++)
					{
						FTransform GrassTransform;
						float GrassX = SpawnLoc.X + FMath::RandRange(-2000.0f, 2000.0f);
						float GrassY = SpawnLoc.Y + FMath::RandRange(-2000.0f, 2000.0f);
						float GrassZ = SplineLoc.Z - 200.0f; // Keep on the flat ground
						
						GrassTransform.SetLocation(FVector(GrassX, GrassY, GrassZ));
						GrassTransform.SetScale3D(FVector(FMath::RandRange(1.0f, 2.0f)));
						GrassTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
						GrassISM->AddInstance(GrassTransform);
					}
				}
				else if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
				{
					// Buildings scale to random height, keep X/Y uniform. Add some rotation variation.
					InstanceTransform.SetScale3D(FVector(1.0f, 1.0f, FMath::RandRange(1.0f, 5.0f)));
					InstanceTransform.SetRotation(FQuat(FRotator(0, FMath::RandBool() ? 0.0f : 90.0f, 0))); // Snap rotation
					SkyscraperISM->AddInstance(InstanceTransform);
				}
		}
	}
}

void AWorldChunk::GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist)
{
	// A simple approach using ISM for tracks (fast).
	float TrackMeshLength = 2500.0f; // 25 meters per mesh segment
	float ScaleX = 1.0f;
	
	if (TrackMeshISM->GetStaticMesh())
	{
		float MeshLength = TrackMeshISM->GetStaticMesh()->GetBoundingBox().GetSize().X;
		if (MeshLength > 10.0f)
		{
			ScaleX = TrackMeshLength / MeshLength;
		}
	}
	
	for (float Dist = StartDist; Dist < EndDist; Dist += TrackMeshLength)
	{
		FVector StartLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		StartRot.Roll = 0.0f;
		FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		
		FTransform TrackTransform1;
		TrackTransform1.SetLocation(StartLoc);
		TrackTransform1.SetRotation(StartRot.Quaternion());
		TrackTransform1.SetScale3D(FVector(ScaleX, 1.0f, 1.0f)); 
		
		FTransform TrackTransform2;
		TrackTransform2.SetLocation(StartLoc + (RightVec * 500.0f)); // Double track offset 5 meters
		TrackTransform2.SetRotation(StartRot.Quaternion());
		TrackTransform2.SetScale3D(FVector(ScaleX, 1.0f, 1.0f)); 
		
		TrackMeshISM->AddInstance(TrackTransform1);
		TrackMeshISM->AddInstance(TrackTransform2);
		
		// Spawn a signal every 200000 meters
		if (FMath::Fmod(Dist, 200000.0f) < TrackMeshLength * 0.5f)
		{
			FTransform SignalTransform;
			SignalTransform.SetLocation(StartLoc + (RightVec * 800.0f)); // 8m to the right
			SignalTransform.SetRotation(StartRot.Quaternion());
			SignalTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
			SignalISM->AddInstance(SignalTransform);
		}
	}
	// --- Procedural Stations ---
	// Roughly every 10 miles (StartDist % 1600000 == 0), spawn a station if we are in an urban zone
	float StationFmod = FMath::Fmod(StartDist, 1600000.0f);
	if (StationFmod < 100.0f || StationFmod > 1600000.0f - 100.0f)
	{
		FVector Loc = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(Loc.X, Loc.Y));
		
		if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
		{
			// Skyscraper scale is massive and blocks view, disabling proxy spawn for now
			// FTransform StationTransform;
			// FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
			// StationTransform.SetLocation(Loc - (RightVec * 300.0f));
			// StationTransform.SetScale3D(FVector(0.5f, 5.0f, 0.2f)); 
			// StationTransform.SetRotation(Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World).Quaternion());
			// SkyscraperISM->AddInstance(StationTransform);
		}
	}

	// --- Overbridges & Level Crossings ---
	// Roughly every 15 miles (StartDist % 2400000 == 0), spawn an intersecting road crossing
	float CrossingFmod = FMath::Fmod(StartDist, 2400000.0f);
	if (StartDist > 100.0f && (CrossingFmod < 100.0f || CrossingFmod > 2400000.0f - 100.0f))
	{
		FVector Loc = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		
		// Phase 13: Level Crossings and AI Boom Gates
		FTransform GateTransformL;
		GateTransformL.SetLocation(Loc - (RightVec * 800.0f));
		GateTransformL.SetRotation(Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World).Quaternion());
		GateTransformL.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

		FTransform GateTransformR;
		GateTransformR.SetLocation(Loc + (RightVec * 800.0f));
		GateTransformR.SetRotation(Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World).Quaternion());
		GateTransformR.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		
		// In full build, this hooks up to AITrainController to rotate the gates 90 degrees when train approaches
		// BoomGateISM->AddInstance(GateTransformL);
		// BoomGateISM->AddInstance(GateTransformR);
		
		// Skyscraper proxy overbridge disabled due to massive scale
		// FTransform BridgeTransform;
		// Loc.Z += 2000.0f;
		// BridgeTransform.SetLocation(Loc);
		// BridgeTransform.SetScale3D(FVector(5.0f, 0.5f, 0.2f));
		// FRotator SplineRot = Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		// SplineRot.Yaw += 90.0f;
		// BridgeTransform.SetRotation(SplineRot.Quaternion());
		// SkyscraperISM->AddInstance(BridgeTransform);
	}
}
