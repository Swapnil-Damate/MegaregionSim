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
		float BrakeCylPSI, int32 Balance, const FString& SignalState, bool bHeadlightsOn,
		float InSpeedLimit = 100.0f, bool bInDeadEnd = false, float InTimeRemaining = 600.0f);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// Slate Widgets
	TSharedPtr<class SOverlay> MainHUDOverlay;
	TSharedPtr<class STextBlock> SlateSpeedText;
	TSharedPtr<class STextBlock> SpeedWarningText;
	TSharedPtr<class STextBlock> DeadEndWarningText;
	TSharedPtr<class STextBlock> ContractTimeText;
	TSharedPtr<class STextBlock> CinematicHintText;

	// Mock Variables
	float SpeedLimit = 100.0f;
	bool bIsApproachingDeadEnd = false;
	float ContractTimeRemaining = 600.0f;


private:
	// Start menu state
	bool bShowStartMenu = true;
	int32 MenuEngineSelection = 0; // 0-3
	int32 MenuCarSelection = 3;    // 0-3  
	int32 MenuCarCountIndex = 4;   // index into {1,2,4,6,8,12}
	int32 MenuWeatherSelection = 0; // 0=Clear, 1=Foggy, 2=Overcast
	int32 MenuTimeSelection = 0; // 0=Day, 1=Night

	void DrawStartMenu();
	void HandleStartMenuInput();

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
