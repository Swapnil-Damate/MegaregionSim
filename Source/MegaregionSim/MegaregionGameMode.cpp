#include "MegaregionGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "TrainPawn.h"
#include "TrainSimHUD.h"
#include "OpenWorldGraphGenerator.h"
#include "MegaregionWeatherSystem.h"
#include "Kismet/GameplayStatics.h"

AMegaregionGameMode::AMegaregionGameMode()
{
	// Set the HUD class to our pure C++ Slate HUD — no Blueprint dependency
	HUDClass = ATrainSimHUD::StaticClass();

	// Set the default pawn to our Train Blueprint if it exists, otherwise fallback to C++ class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_TrainPawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		// Fallback to C++ class if BP doesn't exist yet
		DefaultPawnClass = ATrainPawn::StaticClass();
	}
}

void AMegaregionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// AAA Production Ready Console Commands (Issue 12, Issue 16)
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->ConsoleCommand(TEXT("r.Lumen.Scene.SurfaceCache.AtlasSize 4096"), true);
		PC->ConsoleCommand(TEXT("r.AutoExposure.LowPercent 50"), true);
	}

	// Ensure the OpenWorldGraphGenerator exists in the world BEFORE pawn spawns
	AActor* GraphGen = UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorldGraphGenerator::StaticClass());
	if (!GraphGen)
	{
		FActorSpawnParameters SP;
		SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AOpenWorldGraphGenerator>(AOpenWorldGraphGenerator::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SP);
	}

	// Ensure the Weather System exists in the world BEFORE pawn spawns
	AActor* WeatherSys = UGameplayStatics::GetActorOfClass(GetWorld(), AMegaregionWeatherSystem::StaticClass());
	if (!WeatherSys)
	{
		FActorSpawnParameters SP;
		SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AMegaregionWeatherSystem>(AMegaregionWeatherSystem::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SP);
	}

	// Now call Super so all pawns run BeginPlay() and can find the generated tracks
	Super::BeginPlay();
}
