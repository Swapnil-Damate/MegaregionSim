#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuWidget::StartZenCommute()
{
	// Load the map with Zen Commute settings
	// In a full implementation, we'd pass an options string to the level
	UGameplayStatics::OpenLevel(this, FName("L_MegaregionMaster"), true, TEXT("Mode=ZenCommute"));
}

void UMainMenuWidget::StartGrandLegacy()
{
	UGameplayStatics::OpenLevel(this, FName("L_MegaregionMaster"), true, TEXT("Mode=GrandLegacy"));
}
