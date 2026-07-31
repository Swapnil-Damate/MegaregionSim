#include "MasterSimulationController.h"
#include "GISDataManager.h"
#include "MegaregionZoningGenerator.h"
#include "AutomatedTrainSpawner.h"

#include "Engine/World.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

AMasterSimulationController::AMasterSimulationController()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	PineTreeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PineTreeISM"));
	PineTreeISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PineAsset(TEXT("StaticMesh'/Game/FinalAssets/The_Pine_Tree.The_Pine_Tree'"));
	if (PineAsset.Succeeded()) PineTreeISM->SetStaticMesh(PineAsset.Object);

	SkyscraperISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SkyscraperISM"));
	SkyscraperISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkyAsset(TEXT("StaticMesh'/Game/FinalAssets/Skyscraper.Skyscraper'"));
	if (SkyAsset.Succeeded()) SkyscraperISM->SetStaticMesh(SkyAsset.Object);

	GrassISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GrassISM"));
	GrassISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GrassAsset(TEXT("StaticMesh'/Game/FinalAssets/Grass_patch.Grass_patch'"));
	if (GrassAsset.Succeeded()) GrassISM->SetStaticMesh(GrassAsset.Object);
}

void AMasterSimulationController::BeginPlay()
{
	Super::BeginPlay();

	// NOTE: GenerateEnvironment() has been DISABLED.
	// WorldChunk now handles all procedural environment spawning (trees, grass, buildings)
	// along the spline track. Calling this static grid generator caused a massive
	// duplicate set of floating/overlapping assets at the world origin.
	// GenerateEnvironment();

	UWorld* World = GetWorld();
}

void AMasterSimulationController::GenerateEnvironment()
{
	// Generate procedural environment instances (downscaled to -20000 to 20000 for quick testing)
	for (int32 X = -20000; X <= 20000; X += 5000)
	{
		for (int32 Y = -20000; Y <= 20000; Y += 5000)
		{
			EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(X, Y));
			
			// Calculate Procedural Mountains and Valleys using Perlin Noise
			float NoiseScale = 0.00015f;
			float ZHeight = FMath::PerlinNoise2D(FVector2D(X * NoiseScale, Y * NoiseScale)) * 12000.0f; 
			// 12000.0f means hills up to 120m high and valleys up to 120m deep

			FTransform InstanceTransform;
			InstanceTransform.SetLocation(FVector(X, Y, ZHeight));
			float Scale = FMath::RandRange(0.8f, 1.5f);
			InstanceTransform.SetScale3D(FVector(Scale));
			InstanceTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));

			if (Zone == EZoningClassification::Nature || Zone == EZoningClassification::Village)
			{
				PineTreeISM->AddInstance(InstanceTransform);
				// Scatter Grass nearby
				for (int i = 0; i < 3; i++)
				{
					FTransform GrassTransform;
					float GrassX = X + FMath::RandRange(-2000, 2000);
					float GrassY = Y + FMath::RandRange(-2000, 2000);
					float GrassZ = FMath::PerlinNoise2D(FVector2D(GrassX * NoiseScale, GrassY * NoiseScale)) * 12000.0f;
					
					GrassTransform.SetLocation(FVector(GrassX, GrassY, GrassZ));
					GrassTransform.SetScale3D(FVector(FMath::RandRange(1.0f, 2.0f)));
					GrassTransform.SetRotation(FQuat(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0)));
					GrassISM->AddInstance(GrassTransform);
				}
			}
			else if (Zone == EZoningClassification::UrbanCenter || Zone == EZoningClassification::Suburbs)
			{
				SkyscraperISM->AddInstance(InstanceTransform);
			}
		}
	}
}

#include "MegaregionGameMode.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"

void AMasterSimulationController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bWeatherApplied)
	{
		AMegaregionGameMode* GameMode = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode && GameMode->bStartMenuComplete)
		{
			// Apply Weather
			if (GameMode->SelectedWeather == 1 || GameMode->SelectedWeather == 2) // Foggy or Overcast
			{
				FActorSpawnParameters SpawnParams;
				AExponentialHeightFog* Fog = GetWorld()->SpawnActor<AExponentialHeightFog>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
				if (Fog && Fog->GetComponent())
				{
					Fog->GetComponent()->SetFogDensity(GameMode->SelectedWeather == 1 ? 0.05f : 0.15f);
				}
			}

			// Apply Time Of Day
			if (GameMode->SelectedTimeOfDay == 1) // Night
			{
				TArray<AActor*> Lights;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), Lights);
				for (AActor* Light : Lights)
				{
					Light->SetActorRotation(FRotator(-90.0f, 0, 0)); // Point straight down for dark night
				}
			}

			bWeatherApplied = true;
		}
	}
}
