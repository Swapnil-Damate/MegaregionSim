#include "MegaregionWeatherSystem.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ExponentialHeightFog.h"
#include "NiagaraFunctionLibrary.h"

AMegaregionWeatherSystem::AMegaregionWeatherSystem()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentWeather = EWeatherState::Clear;
	TimeScale = 0.005f; // Re-enabled dynamic time of day!
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
		break;
	case EWeatherState::Rain:
		UE_LOG(LogTemp, Warning, TEXT("Weather System: Rain! Wheel Slip chance increased."));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/FinalAssets/VFX_Rain.VFX_Rain")), FVector::ZeroVector);
		break;
	case EWeatherState::Snow:
		UE_LOG(LogTemp, Error, TEXT("Weather System: Snow! Severe Traction loss."));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/FinalAssets/VFX_Snow.VFX_Snow")), FVector::ZeroVector);
		break;
	case EWeatherState::HeavyFog:
		UE_LOG(LogTemp, Log, TEXT("Weather System: Heavy Fog. Speed reduction recommended."));
		GetWorld()->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass());
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
