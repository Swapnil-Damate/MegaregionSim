#include "TrainHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Fonts/SlateFontInfo.h"
#include "UObject/ConstructorHelpers.h"

void UTrainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UPanelWidget* RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
		WidgetTree->RootWidget = RootPanel;
	}

	if (!HUDContainer)
	{
		HUDContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("HUDContainer"));
		
		if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(RootPanel))
		{
			UCanvasPanelSlot* ContainerSlot = Canvas->AddChildToCanvas(HUDContainer);
			// Anchor to bottom right
			ContainerSlot->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
			ContainerSlot->SetAlignment(FVector2D(1.0f, 1.0f));
			ContainerSlot->SetPosition(FVector2D(-50.0f, -50.0f));
			ContainerSlot->SetSize(FVector2D(400.0f, 300.0f));
		}
		else
		{
			RootPanel->AddChild(HUDContainer);
		}
	}

	if (HUDContainer)
	{
		// Spawn the text elements programmatically if they weren't bound in the Blueprint
		if (!SpeedText) SpeedText = ConstructHUDTextElement(TEXT("SpeedText"), HUDContainer);
		if (!BrakePipeText) BrakePipeText = ConstructHUDTextElement(TEXT("BrakePipeText"), HUDContainer);
		if (!ThrottleText) ThrottleText = ConstructHUDTextElement(TEXT("ThrottleText"), HUDContainer);
		if (!WalletText) WalletText = ConstructHUDTextElement(TEXT("WalletText"), HUDContainer);
		if (!HeadlightText) HeadlightText = ConstructHUDTextElement(TEXT("HeadlightText"), HUDContainer);
		if (!SignalText) SignalText = ConstructHUDTextElement(TEXT("SignalText"), HUDContainer);
		if (!TrackEventText) TrackEventText = ConstructHUDTextElement(TEXT("TrackEventText"), HUDContainer);
	}
}

UTextBlock* UTrainHUDWidget::ConstructHUDTextElement(const FString& ElementName, UVerticalBox* ParentContainer)
{
	UTextBlock* NewText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *ElementName);
	
	// Apply AAA Terminal Styling
	FSlateFontInfo FontInfo = NewText->GetFont();
	FontInfo.Size = 24;
	// We use the default Roboto font, but make it bold and aggressive
	NewText->SetFont(FontInfo);
	NewText->SetColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.8f, 1.0f)); // Cyan/Lime AAA color
	NewText->SetShadowOffset(FVector2D(2.0f, 2.0f));
	NewText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
	
	ParentContainer->AddChild(NewText);
	return NewText;
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float BrakePipePressure, float BrakeCylinderPressure, float ThrottleNotch, int32 EconomyBalance, const FString& ContractName, float CargoIntegrity, const FString& UpcomingTrackEvent, bool bHeadlightsOn, const FString& NextSignal, float SpeedLimit, bool bDeadEnd, float TimeRemaining)
{
	if (SpeedText)
	{
		SpeedText->SetText(FText::FromString(FString::Printf(TEXT("SPEED: %d KM/H"), FMath::RoundToInt(SpeedKmh))));
	}
	
	if (BrakePipeText)
	{
		BrakePipeText->SetText(FText::FromString(FString::Printf(TEXT("BRAKE PIPE: %d PSI"), FMath::RoundToInt(BrakePipePressure))));
	}
	
	if (ThrottleText)
	{
		ThrottleText->SetText(FText::FromString(FString::Printf(TEXT("THROTTLE NOTCH: %d"), FMath::RoundToInt(ThrottleNotch))));
	}
	
	if (WalletText)
	{
		WalletText->SetText(FText::FromString(FString::Printf(TEXT("FUNDS: $%d"), EconomyBalance)));
	}
	
	if (HeadlightText)
	{
		if (bHeadlightsOn)
		{
			HeadlightText->SetText(FText::FromString(TEXT("HEADLIGHTS: [ON]")));
			HeadlightText->SetColorAndOpacity(FLinearColor::Green);
		}
		else
		{
			HeadlightText->SetText(FText::FromString(TEXT("HEADLIGHTS: [OFF]")));
			HeadlightText->SetColorAndOpacity(FLinearColor::Gray);
		}
	}
	
	if (SignalText)
	{
		SignalText->SetText(FText::FromString(FString::Printf(TEXT("NEXT SIGNAL: [%s]"), *NextSignal)));
		if (NextSignal == TEXT("GREEN")) SignalText->SetColorAndOpacity(FLinearColor::Green);
		else if (NextSignal == TEXT("YELLOW")) SignalText->SetColorAndOpacity(FLinearColor::Yellow);
		else SignalText->SetColorAndOpacity(FLinearColor::Red);
	}
	
	if (TrackEventText)
	{
		TrackEventText->SetText(FText::FromString(FString::Printf(TEXT("EVENT: %s"), *UpcomingTrackEvent)));
	}
}

void UTrainHUDWidget::SetTrainPawn(ATrainPawn* Train)
{
}
