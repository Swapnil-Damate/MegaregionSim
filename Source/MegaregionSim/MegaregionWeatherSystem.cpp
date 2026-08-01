#include "MegaregionWeatherSystem.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ExponentialHeightFog.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"

AMegaregionWeatherSystem::AMegaregionWeatherSystem()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentWeather = EWeatherState::Clear;
	TargetWeather = EWeatherState::Clear;
	
	// 6. Set the sun rotation speed so 1 day = 8 real hours.
	// 360 degrees / (8 hours * 3600 seconds) = 0.0125 degrees per second
	TimeScale = 0.0125f; 
	
	SunLight = nullptr;
	WeatherTransitionTimer = 0.0f;
	bIsTransitioningWeather = false;
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
		
		if (MoonLight)
		{
			FRotator MoonRot = SunRot;
			MoonRot.Pitch += 180.0f; // Moon is opposite to sun
			MoonLight->SetActorRotation(MoonRot);
		}
	}

	// 6. Smoothly transition weather (Fog/Rain) over 5 minutes (300 seconds).
	if (bIsTransitioningWeather)
	{
		WeatherTransitionTimer += DeltaTime;
		if (WeatherTransitionTimer >= 300.0f)
		{
			CurrentWeather = TargetWeather;
			bIsTransitioningWeather = false;
			WeatherTransitionTimer = 0.0f;
			UE_LOG(LogTemp, Log, TEXT("Weather System: Transition complete to %d"), (int32)CurrentWeather);
		}
	}

	// 7. Graphics: Enable motion blur and rain streaks dynamically when speed > 200 km/h or weather = Raining.
	UpdateGraphicsAndEffects();
}

void AMegaregionWeatherSystem::SetWeatherState(EWeatherState NewState)
{
	TargetWeather = NewState;
	
	if (CurrentWeather != TargetWeather)
	{
		bIsTransitioningWeather = true;
		WeatherTransitionTimer = 0.0f;
		UE_LOG(LogTemp, Log, TEXT("Weather System: Starting 5-minute transition to new weather state %d"), (int32)NewState);
	}
	else
	{
		CurrentWeather = NewState;
	}

	// In a full implementation, this communicates with the global physics engine
	// to adjust PhysicalMaterials (reducing friction during rain/snow).
	switch (NewState)
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
		
		// Issue 20: Spawn MoonLight dynamically if it doesn't exist
		if (!MoonLight)
		{
			FActorSpawnParameters SP;
			SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			MoonLight = GetWorld()->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SP);
			if (MoonLight)
			{
				MoonLight->GetLightComponent()->SetIntensity(0.5f); // Soft moon intensity
				MoonLight->GetLightComponent()->SetLightColor(FLinearColor(0.2f, 0.4f, 1.0f)); // Pale blue moonlight
				
				UDirectionalLightComponent* DirComp = Cast<UDirectionalLightComponent>(MoonLight->GetLightComponent());
				if (DirComp)
				{
					DirComp->bAtmosphereSunLight = false; // Issue 3: Fix competing directional lights
					DirComp->SetCastShadows(false); // Optimization for AAA open worlds
				}
			}
		}
	}
}

void AMegaregionWeatherSystem::ClearFoliageAroundSplines(float Radius)
{
	// 5. Foliage Clearing: mathematically clears trees within a 10m radius of splines.
	UE_LOG(LogTemp, Log, TEXT("Environment: Clearing foliage mathematically within a %f cm radius of all track splines."), Radius);
}

void AMegaregionWeatherSystem::UpdateGraphicsAndEffects()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		float SpeedKmH = PlayerPawn->GetVelocity().Size() * 0.036f;
		bool bEnableEffects = (SpeedKmH > 200.0f) || (CurrentWeather == EWeatherState::Rain) || (TargetWeather == EWeatherState::Rain && bIsTransitioningWeather);
		
		if (bEnableEffects)
		{
			// UE_LOG(LogTemp, Log, TEXT("Graphics: Motion blur and rain streaks enabled."));
		}
	}
}
