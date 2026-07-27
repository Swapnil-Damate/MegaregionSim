#include "MegaregionWeatherSystem.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

AMegaregionWeatherSystem::AMegaregionWeatherSystem()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentWeather = EWeatherState::Clear;
	TimeScale = 0.0f; // Disabled dynamic time to prevent Lumen exposure clipping
	SunLight = nullptr;
}

void AMegaregionWeatherSystem::BeginPlay()
{
	Super::BeginPlay();
	FindSunLight();
	SetWeatherState(CurrentWeather);
}

void AMegaregionWeatherSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dynamic Time of Day
	if (SunLight)
	{
		FRotator SunRot = SunLight->GetActorRotation();
		SunRot.Pitch += (TimeScale * DeltaTime);
		SunLight->SetActorRotation(SunRot);
	}
}

void AMegaregionWeatherSystem::SetWeatherState(EWeatherState NewState)
{
	CurrentWeather = NewState;

	// In a full implementation, this communicates with the global physics engine
	// to adjust PhysicalMaterials (reducing friction during rain/snow).
	switch (CurrentWeather)
	{
	case EWeatherState::Clear:
		UE_LOG(LogTemp, Log, TEXT("Weather System: Clear. Friction Normal."));
		// Set Friction = 1.0f
		break;
	case EWeatherState::Rain:
		UE_LOG(LogTemp, Warning, TEXT("Weather System: Rain! Wheel Slip chance increased."));
		// Set Friction = 0.6f
		break;
	case EWeatherState::Snow:
		UE_LOG(LogTemp, Error, TEXT("Weather System: Snow! Severe Traction loss."));
		// Set Friction = 0.3f
		break;
	case EWeatherState::HeavyFog:
		UE_LOG(LogTemp, Log, TEXT("Weather System: Heavy Fog. Speed reduction recommended."));
		break;
	}
}

void AMegaregionWeatherSystem::FindSunLight()
{
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);
	
	if (FoundLights.Num() > 0)
	{
		SunLight = Cast<ADirectionalLight>(FoundLights[0]);
	}
}
