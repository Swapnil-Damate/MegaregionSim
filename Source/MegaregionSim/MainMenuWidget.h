#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class MEGAREGIONSIM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Game Modes")
	void StartZenCommute();

	UFUNCTION(BlueprintCallable, Category = "Game Modes")
	void StartGrandLegacy();
};
