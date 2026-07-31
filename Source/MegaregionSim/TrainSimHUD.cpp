#include "TrainSimHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "MegaregionGameMode.h"

ATrainSimHUD::ATrainSimHUD()
{
}

void ATrainSimHUD::UpdateData(float SpeedKmh, float ThrottleNotch, float BrakePipePSI,
	float BrakeCylPSI, int32 Balance, const FString& SignalState, bool bHeadlightsOn)
{
	CachedSpeed      = SpeedKmh;
	CachedThrottle   = ThrottleNotch;
	CachedBrakePipe  = BrakePipePSI;
	CachedBrakeCyl   = BrakeCylPSI;
	CachedBalance    = Balance;
	CachedSignal     = SignalState;
	bCachedHeadlight = bHeadlightsOn;
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
