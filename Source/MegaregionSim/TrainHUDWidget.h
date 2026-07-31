#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainHUDWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class MEGAREGIONSIM_API UTrainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Native UMG Components
	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* HUDContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SpeedText;
	
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* BrakePipeText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ThrottleText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WalletText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* HeadlightText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SignalText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TrackEventText;

	virtual void NativeConstruct() override;

	// High-performance Native UI Update
	void UpdateHUDMetrics(float SpeedKmh, float BrakePipePressure, float BrakeCylinderPressure, float ThrottleNotch, int32 EconomyBalance, const FString& ContractName, float CargoIntegrity = 100.0f, const FString& UpcomingTrackEvent = TEXT("Clear"), bool bHeadlightsOn = false, const FString& NextSignal = TEXT("GREEN"), float SpeedLimit = 100.0f, bool bDeadEnd = false, float TimeRemaining = 600.0f);

	void SetTrainPawn(class ATrainPawn* Train);

private:
	// Helper to mathematically spawn UMG elements if Blueprint is empty
	UTextBlock* ConstructHUDTextElement(const FString& ElementName, UVerticalBox* ParentContainer);
};
