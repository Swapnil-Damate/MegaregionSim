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

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void StartZenCommute();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void StartGrandLegacy();

	// Phase 14: Masterclass Engine Mode
	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void StartMasterclassEngine();
};
