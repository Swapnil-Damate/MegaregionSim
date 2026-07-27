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
		FString FileURI = FString::Printf(TEXT("file://%s"), *FilePath);
		
		UIBrowser->LoadURL(FileURI);
	}
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle, int32 WalletBalance, const FString& ContractDetails)
{
	if (UIBrowser)
	{
		int32 Speed = FMath::RoundToInt(SpeedKmh);
		int32 Pipe = FMath::RoundToInt(PipePSI);
		int32 Cyl = FMath::RoundToInt(CylinderPSI);
		int32 Throt = FMath::RoundToInt(Throttle);

		FString JSCommand = FString::Printf(TEXT("UpdateTelemetry(%d, %d, %d, %d, %d, '%s');"), Speed, Pipe, Cyl, Throt, WalletBalance, *ContractDetails);
		UIBrowser->ExecuteJavascript(JSCommand);
	}
}
