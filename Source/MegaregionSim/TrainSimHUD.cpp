#include "TrainSimHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

ATrainSimHUD::ATrainSimHUD()
{
}

void ATrainSimHUD::UpdateData(float SpeedKmh, float ThrottleNotch, float BrakePipePSI,
	float BrakeCylPSI, int32 Balance, const FString& SignalState, bool bHeadlightsOn)
{
	CachedSpeed     = SpeedKmh;
	CachedThrottle  = ThrottleNotch;
	CachedBrakePipe = BrakePipePSI;
	CachedBrakeCyl  = BrakeCylPSI;
	CachedBalance   = Balance;
	CachedSignal    = SignalState;
	bCachedHeadlight = bHeadlightsOn;
}

void ATrainSimHUD::DrawHUDRow(const FString& Label, const FString& Value,
	float X, float Y, FLinearColor ValueColor)
{
	// Semi-transparent dark background strip
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), X - 10.f, Y - 4.f, 360.f, 30.f);

	// Label in grey
	DrawText(Label, FLinearColor(0.7f, 0.7f, 0.7f, 1.0f), X, Y, GEngine->GetSmallFont(), 1.6f);

	// Value in dynamic colour
	DrawText(Value, ValueColor, X + 200.f, Y, GEngine->GetSmallFont(), 1.6f);
}

void ATrainSimHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	const float PanelX = Canvas->ClipX - 400.f;   // right-side panel
	float Y = Canvas->ClipY - 290.f;               // bottom-right corner
	const float RowH = 34.f;

	// ── Title bar ─────────────────────────────────────────────────────────────
	DrawRect(FLinearColor(0.05f, 0.45f, 0.9f, 0.75f), PanelX - 10.f, Y - 36.f, 380.f, 30.f);
	DrawText(TEXT("⬡  MEGAREGION CAB"), FLinearColor::White, PanelX, Y - 32.f,
		GEngine->GetSmallFont(), 1.7f);

	// ── Speed ─────────────────────────────────────────────────────────────────
	FLinearColor SpeedCol = (CachedSpeed > 200.f) ? FLinearColor::Red
		: (CachedSpeed > 120.f) ? FLinearColor::Yellow
		: FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("SPEED"), FString::Printf(TEXT("%d km/h"), FMath::RoundToInt(CachedSpeed)),
		PanelX, Y, SpeedCol);
	Y += RowH;

	// ── Throttle notch ────────────────────────────────────────────────────────
	FString ThrottleBar;
	int32 NotchInt = FMath::RoundToInt(CachedThrottle);
	for (int i = 0; i < 8; i++) ThrottleBar += (i < NotchInt) ? TEXT("█") : TEXT("░");
	DrawHUDRow(TEXT("THROTTLE"), FString::Printf(TEXT("N%d %s"), NotchInt, *ThrottleBar),
		PanelX, Y, FLinearColor(0.2f, 0.9f, 1.0f, 1.0f));
	Y += RowH;

	// ── Brake pipe ────────────────────────────────────────────────────────────
	FLinearColor BrakeCol = (CachedBrakePipe < 60.f) ? FLinearColor::Red
		: (CachedBrakePipe < 80.f) ? FLinearColor::Yellow
		: FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("BRAKE PIPE"), FString::Printf(TEXT("%.0f PSI"), CachedBrakePipe),
		PanelX, Y, BrakeCol);
	Y += RowH;

	// ── Brake cylinder ────────────────────────────────────────────────────────
	DrawHUDRow(TEXT("BRAKE CYL"), FString::Printf(TEXT("%.0f PSI"), CachedBrakeCyl),
		PanelX, Y, (CachedBrakeCyl > 10.f) ? FLinearColor::Red : FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	Y += RowH;

	// ── Signal ────────────────────────────────────────────────────────────────
	FLinearColor SigCol = (CachedSignal == TEXT("RED")) ? FLinearColor::Red
		: (CachedSignal == TEXT("YELLOW")) ? FLinearColor::Yellow
		: FLinearColor(0.1f, 1.0f, 0.5f, 1.0f);
	DrawHUDRow(TEXT("NEXT SIGNAL"), FString::Printf(TEXT("● %s"), *CachedSignal),
		PanelX, Y, SigCol);
	Y += RowH;

	// ── Headlights & funds ────────────────────────────────────────────────────
	DrawHUDRow(TEXT("HEADLIGHTS"), bCachedHeadlight ? TEXT("[ON]") : TEXT("[OFF]"),
		PanelX, Y,
		bCachedHeadlight ? FLinearColor::Yellow : FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
	Y += RowH;

	DrawHUDRow(TEXT("FUNDS"), FString::Printf(TEXT("$%d"), CachedBalance),
		PanelX, Y, FLinearColor(0.9f, 0.8f, 0.1f, 1.0f));
	Y += RowH;

	// ── Controls reminder ─────────────────────────────────────────────────────
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f), PanelX - 10.f, Y - 4.f, 360.f, 26.f);
	DrawText(TEXT("W/S = Throttle   SPACE = Brakes   H = Horn"),
		FLinearColor(0.55f, 0.55f, 0.55f, 1.0f), PanelX, Y, GEngine->GetSmallFont(), 1.3f);
}
