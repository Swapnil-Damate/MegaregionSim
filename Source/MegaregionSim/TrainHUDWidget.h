#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainHUDWidget.generated.h"

class UWebBrowser;

UCLASS()
class MEGAREGIONSIM_API UTrainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Removed BindWidget so the empty python-generated Blueprint can compile!
	// We will create the WebBrowser dynamically in C++ instead.
	UPROPERTY()
	UWebBrowser* UIBrowser;

	virtual void NativeConstruct() override;

	// Function to be called every frame to update the numbers on the screen via Javascript
	void UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle, int32 WalletBalance, const FString& ContractDetails);
};
