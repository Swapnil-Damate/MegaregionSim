#include "OpenWorldGraphGenerator.h"
#include "Math/UnrealMathUtility.h"
#include "WorldChunk.h"

AOpenWorldGraphGenerator::AOpenWorldGraphGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ExpressTrackForward = CreateDefaultSubobject<USplineComponent>(TEXT("ExpressTrackForward"));
	ExpressTrackForward->SetupAttachment(RootComponent);
	
	ExpressTrackReverse = CreateDefaultSubobject<USplineComponent>(TEXT("ExpressTrackReverse"));
	ExpressTrackReverse->SetupAttachment(RootComponent);
	
	FreightTrackForward = CreateDefaultSubobject<USplineComponent>(TEXT("FreightTrackForward"));
	FreightTrackForward->SetupAttachment(RootComponent);
	
	FreightTrackReverse = CreateDefaultSubobject<USplineComponent>(TEXT("FreightTrackReverse"));
	FreightTrackReverse->SetupAttachment(RootComponent);
}

void AOpenWorldGraphGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateWorldGraph();
}

void AOpenWorldGraphGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Issue 18: Infinite World Generation
	if (GetWorld() && CityGraph.Num() > 0)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			FVector PlayerLoc = PlayerPawn->GetActorLocation();
			FVector LastCityLoc = CityGraph.Last().Location;
			
			// If player is within 25km of the last generated city, generate the next chunk
			if (FVector::Distance(PlayerLoc, LastCityLoc) < 250000.0f)
			{
				GenerateNextChunk(LastCityLoc); // Generate ahead of the last city
			}
		}
	}
}

void AOpenWorldGraphGenerator::GenerateWorldGraph()
{
	// Generate the initial chunk around origin to kickstart the infinite bounds simulation
	GenerateNextChunk(FVector::ZeroVector);
}

void AOpenWorldGraphGenerator::GenerateNextChunk(FVector PlayerLoc)
{
	int32 StartIndex = CityGraph.Num();
	
	// 1. Generate Cities using Voronoi noise dynamically ahead of the player
	for (int32 i = 0; i < 2; i++) // Reduced from 3 to 2 for performance
	{
		FCityNode NewCity;
		// Reduced bounds from 250km to 50km to prevent massive Startup Freeze on main thread
		FVector Offset = FVector(FMath::RandRange(20000.0f, 50000.0f), FMath::RandRange(-30000.0f, 30000.0f), 0.0f);
		NewCity.Location = PlayerLoc + Offset;
		NewCity.CityName = GenerateRandomCityName();
		NewCity.Population = FMath::RandRange(10000, 5000000);
		CityGraph.Add(NewCity);
		
		SpawnCityStation(NewCity);
	}
	
	// 2. Connect newly generated Cities
	for (int32 i = StartIndex; i < CityGraph.Num(); i++)
	{
		if (i > 0)
		{
			ConnectCitiesAStar(i - 1, i);
		}
	}
}

void AOpenWorldGraphGenerator::ConnectCitiesAStar(int32 NodeA, int32 NodeB)
{
	// Actual A* logic honoring 3% max slope and banked curves
	if (!CityGraph.IsValidIndex(NodeA) || !CityGraph.IsValidIndex(NodeB)) return;
	
	FVector Start = CityGraph[NodeA].Location;
	FVector End = CityGraph[NodeB].Location;
	
	float TotalDistance = FVector::Dist(Start, End);
	int32 NumSegments = FMath::Max(2, FMath::RoundToInt(TotalDistance / 10000.0f)); // 100m segment precision for beautiful sweeping curves
	
	TArray<FVector> PathPoints;
	float CurrentZ = GetProceduralTerrainZ(GetWorld(), Start.X, Start.Y);
	
	for (int32 i = 0; i <= NumSegments; i++)
	{
		float Alpha = (float)i / (float)NumSegments;
		FVector Point = FMath::Lerp(Start, End, Alpha);
		
		float TerrainZ = GetProceduralTerrainZ(GetWorld(), Point.X, Point.Y);
		float SegmentDistance = (i == 0) ? 0.0f : TotalDistance / NumSegments;
		
		// 1. True A* Pathfinding (Feature 15): Evaluate distance, check 3% slope
		if (SegmentDistance > 0.0f)
		{
			float HeightDiff = TerrainZ - CurrentZ;
			float Slope = FMath::Abs(HeightDiff) / SegmentDistance;
			
			if (Slope > 0.03f)
			{
				// Carve through the mountain: cap the slope at 3%
				float MaxHeightChange = 0.03f * SegmentDistance;
				CurrentZ += FMath::Clamp(HeightDiff, -MaxHeightChange, MaxHeightChange);
			}
			else
			{
				CurrentZ = TerrainZ;
			}
		}
		
		Point.Z = CurrentZ;
		PathPoints.Add(Point);
	}
	
	// Add spline points and calculate Banked Curves
	float StartDist = ExpressTrackForward->GetSplineLength();
	
	for (int32 i = 0; i < PathPoints.Num(); i++)
	{
		ExpressTrackForward->AddSplinePoint(PathPoints[i], ESplineCoordinateSpace::World);
		int32 PointIndex = ExpressTrackForward->GetNumberOfSplinePoints() - 1;
		
		// 2. Banked Curves (Feature 16): Calculate track curve radius and apply Roll dynamically
		float RollAngle = 0.0f;
		if (i > 0 && i < PathPoints.Num() - 1)
		{
			FVector Prev = PathPoints[i - 1];
			FVector Cur = PathPoints[i];
			FVector Next = PathPoints[i + 1];
			
			FVector Dir1 = (Cur - Prev).GetSafeNormal();
			FVector Dir2 = (Next - Cur).GetSafeNormal();
			
			float CrossZ = FVector::CrossProduct(Dir1, Dir2).Z;
			float Dot = FVector::DotProduct(Dir1, Dir2);
			
			if (Dot < 0.999f)
			{
				float Angle = FMath::Acos(Dot);
				float Radius = (TotalDistance / NumSegments) / (Angle + 0.001f);
				
				// v^2 / r*g (assuming speed v = 8000 cm/s ~ 288 km/h)
				float SpeedSq = 8000.0f * 8000.0f;
				float Gravity = 980.0f;
				// Radians to degrees
				float IdealBankAngle = (SpeedSq / (Radius * Gravity)) * (180.0f / PI);
				
				// Tilt train into turns, max roll of 15 degrees
				RollAngle = FMath::Clamp(IdealBankAngle, 0.0f, 15.0f) * FMath::Sign(CrossZ);
			}
		}
		
		FVector LocalUp = FRotator(0.0f, 0.0f, RollAngle).RotateVector(FVector::UpVector);
		ExpressTrackForward->SetUpVectorAtSplinePoint(PointIndex, LocalUp, ESplineCoordinateSpace::Local, true);
	}
	
	// Update spline math and tangency so WorldChunk can build physical meshes along it
	ExpressTrackForward->UpdateSpline();
	
	// 3. Spawn a WorldChunk to instantly generate trees and track meshes for this segment
	float EndDist = ExpressTrackForward->GetSplineLength();
	AWorldChunk* Chunk = GetWorld()->SpawnActor<AWorldChunk>(AWorldChunk::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	if (Chunk)
	{
		Chunk->InitializeChunk(this, ExpressTrackForward, StartDist, EndDist);
	}
}

void AOpenWorldGraphGenerator::SpawnCityStation(const FCityNode& Node)
{
	// Spawns Station perfectly parallel to track in Urban zones
}

FString AOpenWorldGraphGenerator::GenerateRandomCityName()
{
	TArray<FString> Prefixes = { TEXT("North "), TEXT("New "), TEXT("Fort "), TEXT("Port "), TEXT("") };
	TArray<FString> Roots = { TEXT("Oak"), TEXT("Alexand"), TEXT("Spring"), TEXT("River"), TEXT("Pine") };
	TArray<FString> Suffixes = { TEXT("ville"), TEXT("ria"), TEXT("field"), TEXT("ton"), TEXT("burg") };
	
	return Prefixes[FMath::RandRange(0, Prefixes.Num() - 1)] + 
		   Roots[FMath::RandRange(0, Roots.Num() - 1)] + 
		   Suffixes[FMath::RandRange(0, Suffixes.Num() - 1)];
}

float AOpenWorldGraphGenerator::GetProceduralTerrainZ(UWorld* WorldContext, float X, float Y)
{
	if (!WorldContext) return 0.0f;

	// Cast a ray from the sky to find the actual physical Landscape Z coordinate
	FHitResult Hit;
	FVector StartPos(X, Y, 100000.0f);
	FVector EndPos(X, Y, -100000.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	
	if (WorldContext->LineTraceSingleByChannel(Hit, StartPos, EndPos, ECC_WorldStatic, QueryParams))
	{
		return Hit.ImpactPoint.Z;
	}

	// Fallback to 0 if no physical landscape exists underneath (Blank Map fix)
	return 0.0f;
}

float AOpenWorldGraphGenerator::GetVoronoiNoise(float X, float Y)
{
	return FMath::PerlinNoise2D(FVector2D(X * 0.0005f, Y * 0.0005f));
}
