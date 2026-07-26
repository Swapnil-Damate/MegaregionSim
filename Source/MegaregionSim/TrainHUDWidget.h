#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainHUDWidget.generated.h"

class UWebBrowser;

UCLASS()
class MEGAREGIONSIM_API UTrainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// We use BindWidget so the Unreal Editor knows this maps to the visual UI element
	UPROPERTY(meta = (BindWidget))
	UWebBrowser* UIBrowser;

	virtual void NativeConstruct() override;

	// Function to be called every frame to update the numbers on the screen via Javascript
	void UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle);
};
