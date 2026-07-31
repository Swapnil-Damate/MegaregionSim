#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MegaregionGameMode.generated.h"

UCLASS()
class MEGAREGIONSIM_API AMegaregionGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMegaregionGameMode();
	
	virtual void BeginPlay() override;

	// Train selection — set from HUD start menu before spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Selection")
	int32 SelectedEngineIndex = 0; // 0=Diesel, 1=Steam, 2=Bullet, 3=Metro

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Selection")
	int32 SelectedCarType = 3; // 0=Passenger, 1=Tanker, 2=Hopper, 3=Mixed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Selection")
	int32 SelectedCarCount = 8; // 1, 2, 4, 6, 8, or 12

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Selection")
	int32 SelectedWeather = 0; // 0=Clear, 1=Foggy, 2=Overcast

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Selection")
	int32 SelectedTimeOfDay = 0; // 0=Day, 1=Night

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	bool bStartMenuComplete = false;

	// Turnout distance registry — populated by WorldChunk
	UPROPERTY()
	TArray<float> TurnoutDistances;
};
