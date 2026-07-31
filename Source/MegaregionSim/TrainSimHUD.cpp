#include "TrainSimHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "MegaregionGameMode.h"
#include "Engine/Engine.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SWeakWidget.h"
#include "Styling/CoreStyle.h"

ATrainSimHUD::ATrainSimHUD()
{
}

void ATrainSimHUD::UpdateData(float SpeedKmh, float ThrottleNotch, float BrakePipePSI,
	float BrakeCylPSI, int32 Balance, const FString& SignalState, bool bHeadlightsOn,
	float InSpeedLimit, bool bInDeadEnd, float InTimeRemaining)
{
	CachedSpeed      = SpeedKmh;
	CachedThrottle   = ThrottleNotch;
	CachedBrakePipe  = BrakePipePSI;
	CachedBrakeCyl   = BrakeCylPSI;
	CachedBalance    = Balance;
	CachedSignal     = SignalState;
	bCachedHeadlight = bHeadlightsOn;
	SpeedLimit       = InSpeedLimit;
	bIsApproachingDeadEnd = bInDeadEnd;
	ContractTimeRemaining = InTimeRemaining;
}

void ATrainSimHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(MainHUDOverlay, SOverlay)

		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0, 50, 0, 0))
		[
			SAssignNew(DeadEndWarningText, STextBlock)
			.Text(FText::FromString(TEXT("DEAD END IN 2 KILOMETERS")))
			.ColorAndOpacity(FSlateColor(FLinearColor::Red))
			.Visibility(EVisibility::Hidden)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 36))
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0, 100, 0, 0))
		[
			SAssignNew(SpeedWarningText, STextBlock)
			.Text(FText::FromString(TEXT("SPEED WARNING")))
			.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
			.Visibility(EVisibility::Hidden)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0, 20, 20, 0))
		[
			SAssignNew(ContractTimeText, STextBlock)
			.Text(FText::FromString(TEXT("TIME REMAINING: 10:00")))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 28))
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(20, 0, 0, 20))
		[
			SAssignNew(CinematicHintText, STextBlock)
			.Text(FText::FromString(TEXT("[V] Cinematic Camera")))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(FMargin(20, 20, 0, 0))
		[
			SAssignNew(SlateSpeedText, STextBlock)
			.Text(FText::FromString(TEXT("SPEED: 000 KM/H")))
			.ColorAndOpacity(FSlateColor(FLinearColor::Green))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
		];

		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MainHUDOverlay.ToSharedRef()));
	}
}

void ATrainSimHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GEngine && GEngine->GameViewport && MainHUDOverlay.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MainHUDOverlay.ToSharedRef());
	}
	Super::EndPlay(EndPlayReason);
}

void ATrainSimHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MainHUDOverlay.IsValid() && !bShowStartMenu)
	{
		MainHUDOverlay->SetVisibility(EVisibility::Visible);
		
		if (ContractTimeRemaining > 0)
		{
			ContractTimeRemaining -= DeltaSeconds;
			int32 Mins = FMath::FloorToInt(ContractTimeRemaining / 60.0f);
			int32 Secs = FMath::FloorToInt(FMath::Fmod(ContractTimeRemaining, 60.0f));
			ContractTimeText->SetText(FText::FromString(FString::Printf(TEXT("TIME REMAINING: %02d:%02d"), Mins, Secs)));
		}

		FLinearColor SpeedColor = FLinearColor::Green;
		bool bShowWarning = false;
		FLinearColor WarningColor = FLinearColor::Yellow;

		if (CachedSpeed > SpeedLimit + 15.0f)
		{
			SpeedColor = FLinearColor::Red;
			WarningColor = FLinearColor::Red;
			bShowWarning = true;
		}
		else if (CachedSpeed > SpeedLimit + 5.0f)
		{
			SpeedColor = FLinearColor::Yellow;
			WarningColor = FLinearColor::Yellow;
			bShowWarning = true;
		}

		SlateSpeedText->SetText(FText::FromString(FString::Printf(TEXT("SPEED: %03d KM/H"), FMath::RoundToInt(CachedSpeed))));
		SlateSpeedText->SetColorAndOpacity(FSlateColor(SpeedColor));

		if (bShowWarning)
		{
			float Time = GetWorld()->GetTimeSeconds();
			if (FMath::Fmod(Time, 0.5f) > 0.25f)
			{
				SpeedWarningText->SetVisibility(EVisibility::Visible);
			}
			else
			{
				SpeedWarningText->SetVisibility(EVisibility::Hidden);
			}
			SpeedWarningText->SetColorAndOpacity(FSlateColor(WarningColor));
		}
		else
		{
			SpeedWarningText->SetVisibility(EVisibility::Hidden);
		}

		if (bIsApproachingDeadEnd)
		{
			DeadEndWarningText->SetVisibility(EVisibility::Visible);
		}
		else
		{
			DeadEndWarningText->SetVisibility(EVisibility::Hidden);
		}
	}
	else if (MainHUDOverlay.IsValid() && bShowStartMenu)
	{
		MainHUDOverlay->SetVisibility(EVisibility::Hidden);
	}
}


void ATrainSimHUD::DrawHUDRow(const FString& Label, const FString& Value,
	float X, float Y, FLinearColor ValueColor)
{
	// Dark semi-transparent background strip
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f), X - 10.f, Y - 3.f, 370.f, 28.f);

	// Label in grey — use nullptr font to fall back to engine default (UE5.1-5.5 safe)
	DrawText(Label, FLinearColor(0.65f, 0.65f, 0.65f, 1.0f), X, Y, nullptr, 1.5f);

	// Value in dynamic colour
	DrawText(Value, ValueColor, X + 200.f, Y, nullptr, 1.5f);
}

void ATrainSimHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	if (bShowStartMenu)
	{
		DrawStartMenu();
		HandleStartMenuInput();
		return; // Don't draw cab HUD while start menu is showing
	}

	// Panel anchored to bottom-right corner
	const float PanelW = 380.f;
	const float PanelX = Canvas->ClipX - PanelW - 20.f;
	float       Y      = Canvas->ClipY - 310.f;
	const float RowH   = 32.f;

	// ── Title bar ─────────────────────────────────────────────────────────────
	DrawRect(FLinearColor(0.04f, 0.40f, 0.85f, 0.80f), PanelX - 10.f, Y - 34.f, PanelW, 30.f);
	DrawText(TEXT("MEGAREGION SIM  |  CAB DISPLAY"),
		FLinearColor::White, PanelX, Y - 30.f, nullptr, 1.4f);

	// ── Speed ─────────────────────────────────────────────────────────────────
	FLinearColor SpeedCol = (CachedSpeed > 200.f) ? FLinearColor::Red
		: (CachedSpeed > 120.f)                   ? FLinearColor::Yellow
		:                                            FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("SPEED"), FString::Printf(TEXT("%d km/h"), FMath::RoundToInt(CachedSpeed)),
		PanelX, Y, SpeedCol);
	Y += RowH;

	// ── Throttle notch with visual bar ────────────────────────────────────────
	int32   NotchInt = FMath::RoundToInt(CachedThrottle);
	FString ThrottleBar;
	for (int i = 0; i < 8; i++) ThrottleBar += (i < NotchInt) ? TEXT("|") : TEXT(".");
	DrawHUDRow(TEXT("THROTTLE"),
		FString::Printf(TEXT("N%d  [%s]"), NotchInt, *ThrottleBar),
		PanelX, Y, FLinearColor(0.2f, 0.9f, 1.0f, 1.0f));
	Y += RowH;

	// ── Brake pipe PSI ────────────────────────────────────────────────────────
	FLinearColor PipeCol = (CachedBrakePipe < 60.f) ? FLinearColor::Red
		: (CachedBrakePipe < 80.f)                  ? FLinearColor::Yellow
		:                                              FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("BRAKE PIPE"),
		FString::Printf(TEXT("%.0f PSI"), CachedBrakePipe),
		PanelX, Y, PipeCol);
	Y += RowH;

	// ── Brake cylinder PSI ────────────────────────────────────────────────────
	DrawHUDRow(TEXT("BRAKE CYL"),
		FString::Printf(TEXT("%.0f PSI"), CachedBrakeCyl),
		PanelX, Y, (CachedBrakeCyl > 10.f) ? FLinearColor::Red
		                                     : FLinearColor(0.55f, 0.55f, 0.55f, 1.0f));
	Y += RowH;

	// ── Signal ────────────────────────────────────────────────────────────────
	FLinearColor SigCol = (CachedSignal == TEXT("RED"))    ? FLinearColor::Red
		: (CachedSignal == TEXT("YELLOW"))                 ? FLinearColor::Yellow
		:                                                    FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("NEXT SIGNAL"),
		FString::Printf(TEXT("[ %s ]"), *CachedSignal),
		PanelX, Y, SigCol);
	Y += RowH;

	// ── Headlights ────────────────────────────────────────────────────────────
	DrawHUDRow(TEXT("HEADLIGHTS"),
		bCachedHeadlight ? TEXT("ON") : TEXT("OFF"),
		PanelX, Y,
		bCachedHeadlight ? FLinearColor::Yellow : FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));
	Y += RowH;

	// ── Funds ─────────────────────────────────────────────────────────────────
	DrawHUDRow(TEXT("FUNDS"),
		FString::Printf(TEXT("$%d"), CachedBalance),
		PanelX, Y, FLinearColor(0.95f, 0.82f, 0.1f, 1.0f));
	Y += RowH;

	// ── Control hints ─────────────────────────────────────────────────────────
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f), PanelX - 10.f, Y - 3.f, PanelW, 24.f);
	DrawText(TEXT("W/S: Throttle   Space: Brakes   L: Lights   J: Track"),
		FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), PanelX, Y, nullptr, 1.15f);

	// ── Minimap ───────────────────────────────────────────────────────────────
	const float MinimapRadius = 60.0f;
	FVector2D MinimapCenter(Canvas->ClipX - MinimapRadius - 20.0f, Canvas->ClipY - MinimapRadius - 20.0f);
	for (float my = -MinimapRadius; my <= MinimapRadius; my += 1.0f)
	{
		float mx = FMath::Sqrt(MinimapRadius * MinimapRadius - my * my);
		DrawLine(MinimapCenter.X - mx, MinimapCenter.Y + my, MinimapCenter.X + mx, MinimapCenter.Y + my, FLinearColor(0.0f, 0.0f, 0.0f, 0.5f), 1.0f);
	}
	DrawRect(FLinearColor::Green, MinimapCenter.X - 4.0f, MinimapCenter.Y - 4.0f, 8.0f, 8.0f);
}

void ATrainSimHUD::HandleStartMenuInput()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    
    // Number keys 1-4 for engine selection
    if (PC->WasInputKeyJustPressed(EKeys::One))   MenuEngineSelection = 0;
    if (PC->WasInputKeyJustPressed(EKeys::Two))   MenuEngineSelection = 1;
    if (PC->WasInputKeyJustPressed(EKeys::Three)) MenuEngineSelection = 2;
    if (PC->WasInputKeyJustPressed(EKeys::Four))  MenuEngineSelection = 3;
    
    // Q/E keys for car type
    if (PC->WasInputKeyJustPressed(EKeys::Q))
        MenuCarSelection = (MenuCarSelection + 3) % 4; // prev
    if (PC->WasInputKeyJustPressed(EKeys::E))
        MenuCarSelection = (MenuCarSelection + 1) % 4; // next
    
    // Left/Right arrows for car count
    const int32 CarCounts[] = {1, 2, 4, 6, 8, 12};
    if (PC->WasInputKeyJustPressed(EKeys::Left))
        MenuCarCountIndex = FMath::Max(0, MenuCarCountIndex - 1);
    if (PC->WasInputKeyJustPressed(EKeys::Right))
        MenuCarCountIndex = FMath::Min(5, MenuCarCountIndex + 1);

    // Z/X for Weather
    if (PC->WasInputKeyJustPressed(EKeys::Z))
        MenuWeatherSelection = (MenuWeatherSelection + 2) % 3;
    if (PC->WasInputKeyJustPressed(EKeys::X))
        MenuWeatherSelection = (MenuWeatherSelection + 1) % 3;

    // C/V for Time
    if (PC->WasInputKeyJustPressed(EKeys::C))
        MenuTimeSelection = (MenuTimeSelection + 1) % 2;
    if (PC->WasInputKeyJustPressed(EKeys::V))
        MenuTimeSelection = (MenuTimeSelection + 1) % 2;
    
    // Enter to confirm
    if (PC->WasInputKeyJustPressed(EKeys::Enter))
    {
        bShowStartMenu = false;
        
        // Return control to the game
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
        
        // Store selections in GameMode
        if (AMegaregionGameMode* GM = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->SelectedEngineIndex = MenuEngineSelection;
            GM->SelectedCarType = MenuCarSelection;
            GM->SelectedCarCount = CarCounts[MenuCarCountIndex];
            GM->SelectedWeather = MenuWeatherSelection;
            GM->SelectedTimeOfDay = MenuTimeSelection;
            GM->bStartMenuComplete = true;
        }
    }
}

void ATrainSimHUD::DrawStartMenu()
{
    // Semi-transparent dark background
    DrawRect(FLinearColor(0.0f, 0.02f, 0.05f, 0.92f), 0, 0, Canvas->SizeX, Canvas->SizeY);
    
    float CenterX = Canvas->SizeX * 0.5f;
    float Y = Canvas->SizeY * 0.15f;
    
    // Title
    FString Title = TEXT("MEGAREGION SIM — TRAIN CONFIGURATION");
    DrawText(Title, FLinearColor(0.0f, 0.8f, 1.0f), CenterX - 280.0f, Y, GEngine->GetLargeFont());
    
    Y += 60.0f;
    DrawText(TEXT("═══════════════════════════════════════════"), FLinearColor(0.3f, 0.3f, 0.4f), CenterX - 280.0f, Y, GEngine->GetSmallFont());
    Y += 30.0f;
    
    // Engine selection
    const FString EngineNames[] = {
        TEXT("Diesel Locomotive"),
        TEXT("Steam Locomotive"),
        TEXT("Bullet Train (HSR)"),
        TEXT("Urban Metro Tram")
    };
    
    DrawText(TEXT("ENGINE  [Press 1-4]"), FLinearColor(0.6f, 0.6f, 0.7f), CenterX - 280.0f, Y, GEngine->GetSmallFont());
    Y += 25.0f;
    
    for (int32 i = 0; i < 4; i++)
    {
        FLinearColor Color = (i == MenuEngineSelection) ? FLinearColor(0.0f, 1.0f, 0.5f) : FLinearColor(0.4f, 0.4f, 0.5f);
        FString Prefix = (i == MenuEngineSelection) ? TEXT("► ") : TEXT("  ");
        FString Label = FString::Printf(TEXT("%s%d. %s"), *Prefix, i + 1, *EngineNames[i]);
        DrawText(Label, Color, CenterX - 260.0f, Y, GEngine->GetSmallFont());
        Y += 22.0f;
    }
    
    Y += 15.0f;
    
    // Car type selection
    const FString CarTypeNames[] = {
        TEXT("Passenger Coach"),
        TEXT("Liquid Tanker"),
        TEXT("Bulk Hopper"),
        TEXT("Mixed (Auto)")
    };
    
    DrawText(TEXT("CAR TYPE  [Press Q / E]"), FLinearColor(0.6f, 0.6f, 0.7f), CenterX - 280.0f, Y, GEngine->GetSmallFont());
    Y += 25.0f;
    
    FString CarLabel = FString::Printf(TEXT("◄  %s  ►"), *CarTypeNames[MenuCarSelection]);
    DrawText(CarLabel, FLinearColor(1.0f, 0.85f, 0.0f), CenterX - 260.0f, Y, GEngine->GetSmallFont());
    Y += 35.0f;
    
    // Car count
    const int32 CarCounts[] = {1, 2, 4, 6, 8, 12};
    DrawText(TEXT("CAR COUNT  [Left / Right Arrow]"), FLinearColor(0.6f, 0.6f, 0.7f), CenterX - 280.0f, Y, GEngine->GetSmallFont());
    Y += 25.0f;
    
    FString CountLabel = FString::Printf(TEXT("◄  %d cars  ►"), CarCounts[MenuCarCountIndex]);
    DrawText(CountLabel, FLinearColor(1.0f, 0.85f, 0.0f), CenterX - 260.0f, Y, GEngine->GetSmallFont());
    Y += 35.0f;

    // Weather
    const FString WeatherNames[] = { TEXT("Clear Skies"), TEXT("Morning Fog"), TEXT("Overcast") };
    DrawText(TEXT("WEATHER  [Z / X]"), FLinearColor(0.6f, 0.6f, 0.7f), CenterX + 50.0f, Y - 180.0f, GEngine->GetSmallFont());
    
    FString WeatherLabel = FString::Printf(TEXT("◄  %s  ►"), *WeatherNames[MenuWeatherSelection]);
    DrawText(WeatherLabel, FLinearColor(0.2f, 0.8f, 1.0f), CenterX + 70.0f, Y - 155.0f, GEngine->GetSmallFont());

    // Time
    const FString TimeNames[] = { TEXT("Day"), TEXT("Night") };
    DrawText(TEXT("TIME OF DAY  [C / V]"), FLinearColor(0.6f, 0.6f, 0.7f), CenterX + 50.0f, Y - 100.0f, GEngine->GetSmallFont());
    
    FString TimeLabel = FString::Printf(TEXT("◄  %s  ►"), *TimeNames[MenuTimeSelection]);
    DrawText(TimeLabel, FLinearColor(0.8f, 0.4f, 1.0f), CenterX + 70.0f, Y - 75.0f, GEngine->GetSmallFont());
    
    // Confirm
    DrawText(TEXT("═══════════════════════════════════════════"), FLinearColor(0.3f, 0.3f, 0.4f), CenterX - 280.0f, Y, GEngine->GetSmallFont());
    Y += 30.0f;
    DrawText(TEXT("Press ENTER to start"), FLinearColor(0.0f, 1.0f, 0.5f), CenterX - 120.0f, Y, GEngine->GetLargeFont());
}
