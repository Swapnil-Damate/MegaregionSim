#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TrainHUDWidget.generated.h"

UCLASS()
class MEGAREGIONSIM_API UTrainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PipeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CylinderText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ThrottleText;

	virtual void NativeConstruct() override;

	void UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle);
};
