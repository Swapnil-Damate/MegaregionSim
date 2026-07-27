#include "TrainHUDWidget.h"
#include "WebBrowser.h"
#include "Misc/Paths.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UTrainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!UIBrowser)
	{
		UIBrowser = WidgetTree->ConstructWidget<UWebBrowser>(UWebBrowser::StaticClass(), TEXT("UIBrowser"));
		if (UIBrowser)
		{
			UPanelWidget* RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget);
			if (!RootPanel)
			{
				RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
				WidgetTree->RootWidget = RootPanel;
			}
			
			if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(RootPanel))
			{
				UCanvasPanelSlot* BrowserSlot = Canvas->AddChildToCanvas(UIBrowser);
				BrowserSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
				BrowserSlot->SetOffsets(FMargin(0,0,0,0));
			}
			else
			{
				RootPanel->AddChild(UIBrowser);
			}
		}
	}

	if (UIBrowser)
	{
		// Dynamically find the path to the HTML file in the Content folder
		FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + TEXT("WebUI/HUD.html"));
		FString FileURI = FString::Printf(TEXT("file:///%s"), *FilePath);
		
		UIBrowser->LoadURL(FileURI);
	}
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float BrakePipePressure, float BrakeCylinderPressure, float ThrottleNotch, int32 EconomyBalance, const FString& ContractName, float CargoIntegrity, const FString& UpcomingTrackEvent)
{
	if (UIBrowser)
	{
		int32 Speed = FMath::RoundToInt(SpeedKmh);
		int32 Pipe = FMath::RoundToInt(BrakePipePressure);
		int32 Cyl = FMath::RoundToInt(BrakeCylinderPressure);
		int32 Throt = FMath::RoundToInt(ThrottleNotch);
		int32 Integ = FMath::RoundToInt(CargoIntegrity);

		FString JSCommand = FString::Printf(TEXT("UpdateTelemetry(%d, %d, %d, %d, %d, '%s', %d, '%s');"), Speed, Pipe, Cyl, Throt, EconomyBalance, *ContractName, Integ, *UpcomingTrackEvent);
		UIBrowser->ExecuteJavascript(JSCommand);
	}
}
