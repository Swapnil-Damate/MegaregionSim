#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EconomySubsystem.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuWidget::StartZenCommute()
{
	if (UEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		Economy->AddFunds(10000000); // Infinite money for Zen mode
	}
	UGameplayStatics::OpenLevel(this, FName("L_MegaregionMaster"), true, TEXT("?Mode=ZenCommute"));
}

void UMainMenuWidget::StartGrandLegacy()
{
	if (UEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		Economy->AddFunds(5000); // Starting funds for Legacy mode
	}
	UGameplayStatics::OpenLevel(this, FName("L_MegaregionMaster"), true, TEXT("?Mode=GrandLegacy"));
}
