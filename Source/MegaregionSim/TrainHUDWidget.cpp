#include "TrainHUDWidget.h"
#include "WebBrowser.h"
#include "Misc/Paths.h"

void UTrainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UIBrowser)
	{
		// Dynamically find the path to the HTML file in the Content folder
		FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + TEXT("WebUI/HUD.html"));
		FString FileURI = FString::Printf(TEXT("file://%s"), *FilePath);
		
		UIBrowser->LoadURL(FileURI);
	}
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle)
{
	if (UIBrowser)
	{
		int32 Speed = FMath::RoundToInt(SpeedKmh);
		int32 Pipe = FMath::RoundToInt(PipePSI);
		int32 Cyl = FMath::RoundToInt(CylinderPSI);
		int32 Throt = FMath::RoundToInt(Throttle);

		FString JSCommand = FString::Printf(TEXT("UpdateTelemetry(%d, %d, %d, %d);"), Speed, Pipe, Cyl, Throt);
		UIBrowser->ExecuteJavascript(JSCommand);
	}
}
