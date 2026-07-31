#include "OpenWorldGraphGenerator.h"
#include "Math/UnrealMathUtility.h"

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
}

void AOpenWorldGraphGenerator::GenerateWorldGraph()
{
	// 1. Generate Cities using Voronoi noise
	for (int32 i = 0; i < 10; i++)
	{
		FCityNode NewCity;
		NewCity.Location = FVector(FMath::RandRange(-500000.0f, 500000.0f), FMath::RandRange(-500000.0f, 500000.0f), 0.0f);
		NewCity.CityName = GenerateRandomCityName();
		NewCity.Population = FMath::RandRange(10000, 5000000);
		CityGraph.Add(NewCity);
		
		SpawnCityStation(NewCity);
	}
	
	// 2. Connect Cities
	for (int32 i = 0; i < CityGraph.Num() - 1; i++)
	{
		ConnectCitiesAStar(i, i + 1);
	}
}

void AOpenWorldGraphGenerator::ConnectCitiesAStar(int32 NodeA, int32 NodeB)
{
	// This will be expanded with actual A* logic honoring 3% max slope and 100m tunnels
	if (!CityGraph.IsValidIndex(NodeA) || !CityGraph.IsValidIndex(NodeB)) return;
	
	FVector Start = CityGraph[NodeA].Location;
	FVector End = CityGraph[NodeB].Location;
	
	ExpressTrackForward->AddSplinePoint(Start, ESplineCoordinateSpace::World);
	ExpressTrackForward->AddSplinePoint(End, ESplineCoordinateSpace::World);
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

float AOpenWorldGraphGenerator::GetProceduralTerrainZ(float X, float Y)
{
	// 1000m alpine peaks (Phase 2 constraint)
	float Noise = FMath::PerlinNoise2D(FVector2D(X * 0.0001f, Y * 0.0001f));
	return Noise * 100000.0f; // 1000 meters in UE units
}

float AOpenWorldGraphGenerator::GetVoronoiNoise(float X, float Y)
{
	return FMath::PerlinNoise2D(FVector2D(X * 0.0005f, Y * 0.0005f));
}
