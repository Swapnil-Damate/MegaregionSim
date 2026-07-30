#include "WorldChunk.h"
#include "Components/SplineComponent.h"
#include "MegaregionZoningGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "RailwaySignal.h"
#include "Engine/World.h"

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
	// Enable collision so the train can physically sit on the tracks
	TrackMeshISM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TrackMeshISM->SetCollisionProfileName(TEXT("BlockAll"));
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
	const float NoiseScale   = 0.00015f;
	// Terrain sampling step — every 50m along spline
	const float SplineStep   = 5000.0f;
	// How far each side of track to scatter vegetation (500m each side)
	const float LateralHalf  = 50000.0f;
	const float LateralStep  = 3000.0f; // Every 30m laterally

	for (float Dist = StartDist; Dist < EndDist; Dist += SplineStep)
	{
		FVector  SplineLoc   = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector  SplineRight = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(SplineLoc.X, SplineLoc.Y));

		for (float Offset = -LateralHalf; Offset <= LateralHalf; Offset += LateralStep)
		{
			// Keep a wide clearance corridor around the track itself
			if (FMath::Abs(Offset) < 3000.0f) continue;

			FVector SpawnXY = SplineLoc + (SplineRight * Offset);

			// Apply full Perlin height so vegetation sits on hills and valleys
			float TerrainZ = FMath::PerlinNoise2D(FVector2D(SpawnXY.X * NoiseScale, SpawnXY.Y * NoiseScale)) * 12000.0f;
			FVector SpawnLoc(SpawnXY.X, SpawnXY.Y, TerrainZ);

			FTransform T;
			T.SetLocation(SpawnLoc);

			bool bNature = (Zone == EZoningClassification::Nature || Zone == EZoningClassification::Village);
			bool bUrban  = (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs);

			// Far from track always gets nature regardless of zoning (makes map look alive)
			if (FMath::Abs(Offset) > 20000.0f) bNature = true;

			if (bNature)
			{
				// ── Pine trees ───────────────────────────────────────────────
				T.SetScale3D(FVector(FMath::RandRange(0.7f, 2.2f))); // varied sizes
				T.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
				PineTreeISM->AddInstance(T);

				// ── Dense grass underneath each tree (15 patches) ────────────
				for (int g = 0; g < 15; g++)
				{
					float GX = SpawnLoc.X + FMath::RandRange(-1500.0f, 1500.0f);
					float GY = SpawnLoc.Y + FMath::RandRange(-1500.0f, 1500.0f);
					float GZ = FMath::PerlinNoise2D(FVector2D(GX * NoiseScale, GY * NoiseScale)) * 12000.0f;

					FTransform GT;
					GT.SetLocation(FVector(GX, GY, GZ));
					GT.SetScale3D(FVector(FMath::RandRange(0.5f, 1.8f)));
					GT.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
					GrassISM->AddInstance(GT);
				}
			}
			else if (bUrban)
			{
				// ── Skyscrapers with random height ────────────────────────────
				T.SetScale3D(FVector(1.0f, 1.0f, FMath::RandRange(1.0f, 6.0f)));
				T.SetRotation(FQuat(FRotator(0, FMath::RandBool() ? 0.0f : 90.0f, 0)));
				SkyscraperISM->AddInstance(T);
			}
		}
	}
}

void AWorldChunk::GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist)
{
	const float TrackSegLen = 2500.0f; // 25m per segment

	// Compute scale from mesh bounding box Y-axis (track mesh is modeled sideways)
	float ScaleY = 1.0f;
	if (TrackMeshISM->GetStaticMesh())
	{
		float MeshLen = TrackMeshISM->GetStaticMesh()->GetBoundingBox().GetSize().Y;
		if (MeshLen > 1.0f) // safe guard: only override if mesh bounds are non-zero
		{
			ScaleY = TrackSegLen / MeshLen;
		}
	}
	// Hard fallback: if asset failed to load or bounds came back zero, use 1.0
	if (ScaleY <= 0.0f || ScaleY > 1000.0f) ScaleY = 1.0f;

	for (float Dist = StartDist; Dist < EndDist; Dist += TrackSegLen)
	{
		FVector  StartLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		StartRot.Roll = 0.0f;
		// Track mesh is built sideways in Blender — rotate 90° so it faces forward
		StartRot.Yaw += 90.0f;

		FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

		// ── Left rail ────────────────────────────────────────────────────────
		FTransform T1;
		T1.SetLocation(StartLoc - (RightVec * 750.0f));  // 7.5m left of centre
		T1.SetRotation(StartRot.Quaternion());
		T1.SetScale3D(FVector(1.0f, ScaleY, 1.0f));
		TrackMeshISM->AddInstance(T1);

		// ── Right rail ───────────────────────────────────────────────────────
		FTransform T2;
		T2.SetLocation(StartLoc + (RightVec * 750.0f));  // 7.5m right of centre
		T2.SetRotation(StartRot.Quaternion());
		T2.SetScale3D(FVector(1.0f, ScaleY, 1.0f));
		TrackMeshISM->AddInstance(T2);

		// ── Parallel second track (far-side) 20m right ───────────────────────
		FTransform T3;
		T3.SetLocation(StartLoc + (RightVec * 2750.0f));
		T3.SetRotation(StartRot.Quaternion());
		T3.SetScale3D(FVector(1.0f, ScaleY, 1.0f));
		TrackMeshISM->AddInstance(T3);

		FTransform T4;
		T4.SetLocation(StartLoc + (RightVec * 4250.0f));
		T4.SetRotation(StartRot.Quaternion());
		T4.SetScale3D(FVector(1.0f, ScaleY, 1.0f));
		TrackMeshISM->AddInstance(T4);

		// ── Signal every 2km via a real ARailwaySignal actor ─────────────────
		if (GetWorld() && FMath::Fmod(Dist, 200000.0f) < TrackSegLen * 0.5f)
		{
			FVector SignalLoc = StartLoc + (RightVec * 1500.0f);
			FActorSpawnParameters SP;
			SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ARailwaySignal* Sig = GetWorld()->SpawnActor<ARailwaySignal>(
				ARailwaySignal::StaticClass(), SignalLoc, StartRot, SP);
			if (Sig) Sig->SetActorScale3D(FVector(1.0f));
		}
	}

	// ── Procedural Stations (every 10 miles in Urban zones) ──────────────────
	float StationFmod = FMath::Fmod(StartDist, 1600000.0f);
	if (StationFmod < 100.0f)
	{
		FVector Loc = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(Loc.X, Loc.Y));
		if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
		{
			// Use skyscraper ISM as platform proxy at small scale until real asset is available
			FVector Right = Spline->GetRightVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
			FTransform StTr;
			StTr.SetLocation(Loc + Right * 1200.0f);
			StTr.SetRotation(Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World).Quaternion());
			StTr.SetScale3D(FVector(3.0f, 8.0f, 0.1f)); // flat platform slab
			SkyscraperISM->AddInstance(StTr);
		}
	}
}
