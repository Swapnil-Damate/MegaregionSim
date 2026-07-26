#include "TrainHUDWidget.h"
#include "Components/TextBlock.h"

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle)
{
	// Ensure the text blocks exist before trying to update them (prevents crashing if UI isn't fully designed yet)
	if (SpeedText)
	{
		SpeedText->SetText(FText::AsNumber(FMath::RoundToInt(SpeedKmh)));
	}
	
	if (BrakePipePressureText)
	{
		BrakePipePressureText->SetText(FText::AsNumber(FMath::RoundToInt(PipePSI)));
	}

	if (BrakeCylinderPressureText)
	{
		BrakeCylinderPressureText->SetText(FText::AsNumber(FMath::RoundToInt(CylinderPSI)));
	}

	if (ThrottleNotchText)
	{
		ThrottleNotchText->SetText(FText::AsNumber(FMath::RoundToInt(Throttle)));
	}
}
