#include "TrainSimHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"

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
