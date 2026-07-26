#include "TrainHUDWidget.h"

void UTrainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle)
{
	if (SpeedText)
	{
		SpeedText->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.1f km/h"), SpeedKmh)));
	}
	
	if (PipeText)
	{
		PipeText->SetText(FText::FromString(FString::Printf(TEXT("Brake Pipe: %.1f PSI"), PipePSI)));
	}
	
	if (CylinderText)
	{
		CylinderText->SetText(FText::FromString(FString::Printf(TEXT("Cylinders: %.1f PSI"), CylinderPSI)));
	}
	
	if (ThrottleText)
	{
		ThrottleText->SetText(FText::FromString(FString::Printf(TEXT("Throttle: Notch %.0f"), Throttle)));
	}
}
