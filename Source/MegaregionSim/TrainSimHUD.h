#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TrainSimHUD.generated.h"

class ATrainPawn;

UCLASS()
class MEGAREGIONSIM_API ATrainSimHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATrainSimHUD();

	virtual void DrawHUD() override;

	// Called by TrainPawn every tick to push latest data
	void UpdateData(float SpeedKmh, float ThrottleNotch, float BrakePipePSI,
		float BrakeCylPSI, int32 Balance, const FString& SignalState, bool bHeadlightsOn);

private:
	// Latest cached values for thread-safe DrawHUD rendering
	float CachedSpeed        = 0.0f;
	float CachedThrottle     = 0.0f;
	float CachedBrakePipe    = 90.0f;
	float CachedBrakeCyl     = 0.0f;
	int32 CachedBalance      = 0;
	FString CachedSignal     = TEXT("GREEN");
	bool bCachedHeadlight    = false;

	// Helper to draw a labelled HUD row with background
	void DrawHUDRow(const FString& Label, const FString& Value,
		float X, float Y, FLinearColor ValueColor);
};
