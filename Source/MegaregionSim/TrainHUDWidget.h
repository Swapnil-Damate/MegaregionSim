#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class MEGAREGIONSIM_API UTrainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// We use BindWidget so the Unreal Editor knows these map to the visual UI elements
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BrakePipePressureText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BrakeCylinderPressureText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ThrottleNotchText;

	// Function to be called every frame to update the numbers on the screen
	void UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle);
};
