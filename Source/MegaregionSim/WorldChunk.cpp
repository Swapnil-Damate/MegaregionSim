#include "WorldChunk.h"
#include "Components/SplineComponent.h"
#include "MegaregionZoningGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "RailwaySignal.h"
#include "Engine/World.h"
#include "Components/TextRenderComponent.h"

AWorldChunk::AWorldChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	PineTreeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PineTreeISM"));
	PineTreeISM->SetupAttachment(RootComponent);
	PineTreeISM->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PineAsset(TEXT("StaticMesh'/Game/FinalAssets/The_Pine_Tree.The_Pine_Tree'"));
	if (PineAsset.Succeeded()) PineTreeISM->SetStaticMesh(PineAsset.Object);

	BroadleafTreeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BroadleafTreeISM"));
	BroadleafTreeISM->SetupAttachment(RootComponent);
	BroadleafTreeISM->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BroadleafAsset(TEXT("StaticMesh'/Game/FinalAssets/Broadleaf_Tree.Broadleaf_Tree'"));
	if (BroadleafAsset.Succeeded()) BroadleafTreeISM->SetStaticMesh(BroadleafAsset.Object);

	GrassISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GrassISM"));
	GrassISM->SetupAttachment(RootComponent);
	GrassISM->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GrassAsset(TEXT("StaticMesh'/Game/FinalAssets/Grass_patch.Grass_patch'"));
	if (GrassAsset.Succeeded()) GrassISM->SetStaticMesh(GrassAsset.Object);

	ConcreteISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ConcreteISM"));
	ConcreteISM->SetupAttachment(RootComponent);
	ConcreteISM->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConcreteAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (ConcreteAsset.Succeeded()) ConcreteISM->SetStaticMesh(ConcreteAsset.Object);

	SkyscraperISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SkyscraperISM"));
	SkyscraperISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkyAsset(TEXT("StaticMesh'/Game/FinalAssets/Skyscraper.Skyscraper'"));
	if (SkyAsset.Succeeded()) SkyscraperISM->SetStaticMesh(SkyAsset.Object);
	
	SmallHouseISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SmallHouseISM"));
	SmallHouseISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SmallHouseAsset(TEXT("StaticMesh'/Game/FinalAssets/Small_House_.Small_House_'"));
	if (SmallHouseAsset.Succeeded()) SmallHouseISM->SetStaticMesh(SmallHouseAsset.Object);

	FactoryISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FactoryISM"));
	FactoryISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FactoryAsset(TEXT("StaticMesh'/Game/FinalAssets/Factory_building.Factory_building'"));
	if (FactoryAsset.Succeeded()) FactoryISM->SetStaticMesh(FactoryAsset.Object);

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

	BridgeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BridgeISM"));
	BridgeISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BridgeAsset(TEXT("StaticMesh'/Game/FinalAssets/Bridge_Mesh.Bridge_Mesh'"));
	if (BridgeAsset.Succeeded()) BridgeISM->SetStaticMesh(BridgeAsset.Object);

	SignalISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SignalISM"));
	SignalISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SignalAsset(TEXT("StaticMesh'/Game/FinalAssets/Modern_LED_Signal.Modern_LED_Signal'"));
	if (SignalAsset.Succeeded()) SignalISM->SetStaticMesh(SignalAsset.Object);

	CrossingISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CrossingISM"));
	CrossingISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrossingAsset(TEXT("StaticMesh'/Game/FinalAssets/Railway_Crossing.Railway_Crossing'"));
	if (CrossingAsset.Succeeded()) CrossingISM->SetStaticMesh(CrossingAsset.Object);

	CatenaryISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CatenaryISM"));
	CatenaryISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CatenaryAsset(TEXT("StaticMesh'/Game/FinalAssets/Catenary_Pole.Catenary_Pole'"));
	if (CatenaryAsset.Succeeded()) CatenaryISM->SetStaticMesh(CatenaryAsset.Object);

	MilepostISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MilepostISM"));
	MilepostISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MilepostAsset(TEXT("StaticMesh'/Game/FinalAssets/Milepost_Marker.Milepost_Marker'"));
	if (MilepostAsset.Succeeded()) MilepostISM->SetStaticMesh(MilepostAsset.Object);

	TurnoutISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TurnoutISM"));
	TurnoutISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TurnoutAsset(TEXT("StaticMesh'/Game/FinalAssets/Turnout_Mesh.Turnout_Mesh'"));
	if (TurnoutAsset.Succeeded()) TurnoutISM->SetStaticMesh(TurnoutAsset.Object);

	StationISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StationISM"));
	StationISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StationAsset(TEXT("StaticMesh'/Game/FinalAssets/Large_Passenger_Terminal.Large_Passenger_Terminal'"));
	if (StationAsset.Succeeded()) StationISM->SetStaticMesh(StationAsset.Object);

	RuralStationISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RuralStationISM"));
	RuralStationISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RuralStationAsset(TEXT("StaticMesh'/Game/FinalAssets/Small_Rural_Station.Small_Rural_Station'"));
	if (RuralStationAsset.Succeeded()) RuralStationISM->SetStaticMesh(RuralStationAsset.Object);
}

void AWorldChunk::InitializeChunk(AOpenWorldGraphGenerator* Generator, USplineComponent* InSpline, float StartDistance, float EndDistance)
{
	if (!Generator || !InSpline) return;
	GenerateTerrainInstances(InSpline, StartDistance, EndDistance);
	GenerateTrackSplineMeshes(InSpline, StartDistance, EndDistance);
}

#include "OpenWorldGraphGenerator.h"

static float GetGroundHeightForChunk(UWorld* World, float X, float Y, float DefaultZ)
{
	return AOpenWorldGraphGenerator::GetProceduralTerrainZ(World, X, Y);
}

void AWorldChunk::GenerateTerrainInstances(USplineComponent* Spline, float StartDist, float EndDist)
{
	const float NoiseScale   = 0.00015f;
	// Terrain sampling step — increased to 100m to fix freeze
	const float SplineStep   = 10000.0f;
	// How far each side of track to scatter vegetation (reduced to 200m)
	const float LateralHalf  = 20000.0f;
	const float LateralStep  = 8000.0f; // Every 80m laterally to reduce instance count

	for (float Dist = StartDist; Dist < EndDist; Dist += SplineStep)
	{
		FVector  SplineLoc   = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector  SplineRight = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(SplineLoc.X, SplineLoc.Y));

		for (float Offset = -LateralHalf; Offset <= LateralHalf; Offset += LateralStep)
		{
				// Keep a massive clearance corridor around the track for AAA quality (Issue 10)
				if (FMath::Abs(Offset) < 15000.0f) continue;

			FVector SpawnXY = SplineLoc + (SplineRight * Offset);

			// Find exact ground Z from landscape collision
			float GroundZ = GetGroundHeightForChunk(GetWorld(), SpawnXY.X, SpawnXY.Y, SplineLoc.Z);
			FVector SpawnLoc(SpawnXY.X, SpawnXY.Y, GroundZ);

			FTransform T;
			T.SetLocation(SpawnLoc);

			bool bNature = (Zone == EZoningClassification::Nature || Zone == EZoningClassification::Village);
			bool bUrban  = (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs);

			// Far from track always gets nature regardless of zoning (makes map look alive)
			if (FMath::Abs(Offset) > 20000.0f) bNature = true;

			if (bNature)
			{
				bool bUseBroadleaf = FMath::RandBool();
				T.SetScale3D(FVector(FMath::RandRange(0.7f, 2.2f)));
				T.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
				if (bUseBroadleaf)
					BroadleafTreeISM->AddInstance(T);
				else
					PineTreeISM->AddInstance(T);
				
				// Massive grass density underneath each tree (150 patches in 50m radius)
				// Issue 7 & 10: AAA Jittered Grid Grass Generation
				const float GridStep = 250.0f;
				for (float XOffset = -8000.0f; XOffset <= 8000.0f; XOffset += GridStep)
				{
					for (float YOffset = -8000.0f; YOffset <= 8000.0f; YOffset += GridStep)
					{
						// Add a tiny random jitter to the grid coordinate to make it look organic
						float GX = SpawnLoc.X + XOffset + FMath::RandRange(-50.0f, 50.0f);
						float GY = SpawnLoc.Y + YOffset + FMath::RandRange(-50.0f, 50.0f);
						
						// Keep grass strictly off the tracks (at least 35m from spline point)
						if (FVector2D::Distance(FVector2D(GX, GY), FVector2D(SplineLoc.X, SplineLoc.Y)) < 3500.0f) continue;
						
						float GZ = GetGroundHeightForChunk(GetWorld(), GX, GY, SpawnLoc.Z);
						
						FTransform GrassTransform;
						GrassTransform.SetLocation(FVector(GX, GY, GZ));
						
						if (bUrban)
						{
							// Procedural concrete paving
							GrassTransform.SetScale3D(FVector(GridStep / 100.0f, GridStep / 100.0f, 1.0f));
							GrassTransform.SetRotation(FQuat(FRotator(0, 0, 0)));
							ConcreteISM->AddInstance(GrassTransform);
						}
						else
						{
							GrassTransform.SetScale3D(FVector(FMath::RandRange(1.0f, 2.0f)));
							GrassTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
							GrassISM->AddInstance(GrassTransform);
						}
					}
				}
			}
			else if (bUrban)
			{
				if (FMath::Abs(Offset) < 15000.0f)
				{
					// Close to track = small houses and factories
					if (FMath::RandBool())
					{
						T.SetScale3D(FVector(FMath::RandRange(0.8f, 1.5f)));
						T.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
						SmallHouseISM->AddInstance(T);
					}
					else
					{
						T.SetScale3D(FVector(FMath::RandRange(1.0f, 2.0f)));
						T.SetRotation(FQuat(FRotator(0, FMath::RandBool() ? 0.0f : 90.0f, 0)));
						FactoryISM->AddInstance(T);
					}
				}
				else
				{
					// Far from track = skyscrapers
					T.SetScale3D(FVector(1.0f, 1.0f, FMath::RandRange(1.0f, 6.0f)));
					T.SetRotation(FQuat(FRotator(0, FMath::RandBool() ? 0.0f : 90.0f, 0)));
					SkyscraperISM->AddInstance(T);
				}
			}
		}
	}
}

void AWorldChunk::GenerateTrackSplineMeshes(USplineComponent* Spline, float StartDist, float EndDist)
{
	const float TrackSegLen = 2500.0f; // 25m per segment

	float ScaleAlongTrack = 1.0f;
	if (TrackMeshISM->GetStaticMesh())
	{
		FVector MeshSize = TrackMeshISM->GetStaticMesh()->GetBoundingBox().GetSize();
		float MeshLen = FMath::Max3(MeshSize.X, MeshSize.Y, MeshSize.Z);
		if (MeshLen > 1.0f) // safe guard: only override if mesh bounds are non-zero
		{
			ScaleAlongTrack = TrackSegLen / MeshLen;
		}
	}
	// Hard fallback: if asset failed to load or bounds came back zero, use 1.0
	if (ScaleAlongTrack <= 0.0f || ScaleAlongTrack > 1000.0f) ScaleAlongTrack = 1.0f;

	for (float Dist = StartDist; Dist < EndDist; Dist += TrackSegLen)
	{
		float NextDist = FMath::Min(Dist + TrackSegLen, EndDist);
		
		FVector  StartLoc = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FVector  StartTan = Spline->GetTangentAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		
		FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		StartRot.Roll = 0.0f;
		
		FVector  EndLoc = Spline->GetLocationAtDistanceAlongSpline(NextDist, ESplineCoordinateSpace::World);
		FVector  EndTan = Spline->GetTangentAtDistanceAlongSpline(NextDist, ESplineCoordinateSpace::World);

		FVector RightVec = Spline->GetRightVectorAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

		// Z-Snapping to physical progressive map (AAA Fix)
		float GroundZ = GetGroundHeightForChunk(GetWorld(), StartLoc.X, StartLoc.Y, StartLoc.Z);
		float EndGroundZ = GetGroundHeightForChunk(GetWorld(), EndLoc.X, EndLoc.Y, EndLoc.Z);
		
		// If ground drops significantly, treat as bridge/tunnel
		float ZDiff = StartLoc.Z - GroundZ;
		bool bIsBridge = (ZDiff > 500.0f);
		bool bIsTunnel = (ZDiff < -500.0f);

		// Always elevate tracks above terrain/bridge by 20.0f to prevent Z-fighting clipping!
		StartLoc.Z = StartLoc.Z + 20.0f;
		EndLoc.Z = EndLoc.Z + 20.0f;

		// 1. Spawn Curved Spline Mesh for Main Track
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->RegisterComponent();
		SplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		SplineMesh->SetStaticMesh(TrackMeshISM->GetStaticMesh());
		SplineMesh->SetStartAndEnd(StartLoc, StartTan.GetClampedToMaxSize(TrackSegLen), EndLoc, EndTan.GetClampedToMaxSize(TrackSegLen), true);
		TrackSplineMeshes.Add(SplineMesh);

		// Parallel track
		FVector ParallelStartLoc = StartLoc + (RightVec * 3500.0f);
		FVector ParallelEndLoc = EndLoc + (Spline->GetRightVectorAtDistanceAlongSpline(NextDist, ESplineCoordinateSpace::World) * 3500.0f);
		
		FVector ParallelLoc = ParallelStartLoc;
		float ParallelGroundZ = GetGroundHeightForChunk(GetWorld(), ParallelLoc.X, ParallelLoc.Y, ParallelLoc.Z);
		float ParallelZDiff = ParallelLoc.Z - ParallelGroundZ;
		bool bParallelIsBridge = (ParallelZDiff > 500.0f);
		bool bParallelIsTunnel = (ParallelZDiff < -500.0f);
		
		USplineMeshComponent* ParallelSplineMesh = NewObject<USplineMeshComponent>(this);
		ParallelSplineMesh->RegisterComponent();
		ParallelSplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		ParallelSplineMesh->SetStaticMesh(TrackMeshISM->GetStaticMesh());
		ParallelSplineMesh->SetStartAndEnd(ParallelStartLoc, StartTan.GetClampedToMaxSize(TrackSegLen), ParallelEndLoc, EndTan.GetClampedToMaxSize(TrackSegLen), true);
		TrackSplineMeshes.Add(ParallelSplineMesh);

		// Bridge
		if (bIsBridge)
		{
			FTransform BridgeTransform;
			BridgeTransform.SetLocation(StartLoc);
			BridgeTransform.SetRotation(StartRot.Quaternion());
			BridgeTransform.SetScale3D(FVector(ScaleAlongTrack));
			BridgeISM->AddInstance(BridgeTransform);
		}
		else if (bIsTunnel)
		{
			FTransform TunnelTransform;
			TunnelTransform.SetLocation(StartLoc + FVector(0.0f, 0.0f, -20.0f));
			TunnelTransform.SetRotation(StartRot.Quaternion());
			// Issue 6: Increase Tunnel Z-Scale drastically to act as a barricade wall
			TunnelTransform.SetScale3D(FVector(ScaleAlongTrack, ScaleAlongTrack, ScaleAlongTrack * 4.0f));
			TunnelISM->AddInstance(TunnelTransform);
		}

		// Same for parallel track bridge/tunnel
		if (bParallelIsBridge)
		{
			FTransform BridgeTransform;
			BridgeTransform.SetLocation(ParallelLoc);
			BridgeTransform.SetRotation(StartRot.Quaternion());
			BridgeTransform.SetScale3D(FVector(ScaleAlongTrack));
			BridgeISM->AddInstance(BridgeTransform);
		}
		else if (bParallelIsTunnel)
		{
			FTransform TunnelTransform;
			TunnelTransform.SetLocation(ParallelLoc + FVector(0.0f, 0.0f, -20.0f));
			TunnelTransform.SetRotation(StartRot.Quaternion());
			TunnelTransform.SetScale3D(FVector(ScaleAlongTrack, ScaleAlongTrack, ScaleAlongTrack * 4.0f));
			TunnelISM->AddInstance(TunnelTransform);
		}

		// Turnout every 5km
		if (FMath::Fmod(Dist, 500000.0f) < TrackSegLen * 0.5f)
		{
			// Turnout between main and parallel track
			FVector TurnoutLoc = StartLoc + (RightVec * 1750.0f); // halfway between main and parallel
			FTransform TurnoutTransform;
			TurnoutTransform.SetLocation(TurnoutLoc);
			TurnoutTransform.SetRotation(StartRot.Quaternion());
			TurnoutTransform.SetScale3D(FVector(1.0f));
			TurnoutISM->AddInstance(TurnoutTransform);
			
			// Phase 3: Holographic Switch Arrows
			UStaticMeshComponent* Arrow = NewObject<UStaticMeshComponent>(this);
			Arrow->RegisterComponent();
			Arrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			Arrow->SetWorldLocation(TurnoutLoc + FVector(0.0f, 0.0f, 500.0f)); // Hover above
			Arrow->SetWorldRotation(FRotator(-90.0f, StartRot.Yaw, 0.0f)); // Point down
			Arrow->SetWorldScale3D(FVector(3.0f));
			
			UStaticMesh* ArrowAsset = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"));
			if (ArrowAsset) Arrow->SetStaticMesh(ArrowAsset);
			
			SwitchArrows.Add(Arrow);
		}

		// Catenary poles every 250m (25000 units)
		if (FMath::Fmod(Dist, 25000.0f) < TrackSegLen * 0.5f)
		{
		// Catenary logic
		int i = FMath::RoundToInt(Dist / 25000.0f);
		if (i % 2 == 0) // Every 200m
		{
			FTransform CatenaryTransform;
			CatenaryTransform.SetLocation(StartLoc - (RightVec * 400.0f));
			FRotator CatenaryRot = StartRot;
			CatenaryRot.Yaw += 180.0f; // Issue 6: Rotate 180 degrees
			CatenaryTransform.SetRotation(CatenaryRot.Quaternion());
			CatenaryISM->AddInstance(CatenaryTransform);
		}
		}

		// Milepost every 1km (100000 units)
		if (FMath::Fmod(Dist, 100000.0f) < TrackSegLen * 0.5f)
		{
		int i = FMath::RoundToInt(Dist / 100000.0f);
		if (i % 5 == 0) // Every 500m
		{
			// Issue 11: Push mileposts out 15m to prevent clipping
			FVector MilepostLoc = StartLoc - (RightVec * 1500.0f);
			FTransform MPTransform;
			MPTransform.SetLocation(MilepostLoc);
			MPTransform.SetRotation(StartRot.Quaternion());
			MilepostISM->AddInstance(MPTransform);
		}
		}

		// Railway crossing every 4km (400000 units)
		if (FMath::Fmod(Dist, 400000.0f) < TrackSegLen * 0.5f)
		{
			FTransform CrossingTransform;
			CrossingTransform.SetLocation(StartLoc);
			CrossingTransform.SetRotation(StartRot.Quaternion());
			CrossingTransform.SetScale3D(FVector(1.0f));
			CrossingISM->AddInstance(CrossingTransform);
		}

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

	// Procedural Stations every 16km
	float StationFmod = FMath::Fmod(StartDist, 1600000.0f);
	if (StationFmod < 100.0f)
	{
		FVector Loc = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(Loc.X, Loc.Y));
		FVector Right = Spline->GetRightVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		FRotator SplineRot = Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
		
		if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
		{
			// Large passenger terminal
			FVector StationLoc = Loc + Right * 2000.0f;
			StationLoc.Z = GetGroundHeightForChunk(GetWorld(), StationLoc.X, StationLoc.Y, Loc.Z);
			
			FTransform StTr;
			StTr.SetLocation(StationLoc); // 20m from track
			StTr.SetRotation(SplineRot.Quaternion());
			StTr.SetScale3D(FVector(1.0f));
			StationISM->AddInstance(StTr);
			
			// Phase 3: 3D Station Name
			UTextRenderComponent* TextComp = NewObject<UTextRenderComponent>(this);
			TextComp->RegisterComponent();
			TextComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			TextComp->SetWorldLocation(StationLoc + FVector(0.0f, 0.0f, 2000.0f));
			TextComp->SetWorldRotation(SplineRot);
			TextComp->SetText(FText::FromString(TEXT("GRAND CENTRAL TERMINAL")));
			TextComp->SetWorldSize(500.0f);
			TextComp->SetHorizontalAlignment(EHTA_Center);
			StationNames.Add(TextComp);
		}
		else
		{
			// Small rural station
			FVector StationLoc = Loc + Right * 1500.0f;
			StationLoc.Z = GetGroundHeightForChunk(GetWorld(), StationLoc.X, StationLoc.Y, Loc.Z);
			
			FTransform StTr;
			StTr.SetLocation(StationLoc); // 15m from track
			StTr.SetRotation(SplineRot.Quaternion());
			StTr.SetScale3D(FVector(1.0f));
			RuralStationISM->AddInstance(StTr);
			
			// Phase 3: 3D Station Name
			UTextRenderComponent* TextComp = NewObject<UTextRenderComponent>(this);
			TextComp->RegisterComponent();
			TextComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			TextComp->SetWorldLocation(StationLoc + FVector(0.0f, 0.0f, 1000.0f));
			TextComp->SetWorldRotation(SplineRot);
			TextComp->SetText(FText::FromString(TEXT("RURAL STATION")));
			TextComp->SetWorldSize(250.0f);
			TextComp->SetHorizontalAlignment(EHTA_Center);
			StationNames.Add(TextComp);
		}
	}
}
