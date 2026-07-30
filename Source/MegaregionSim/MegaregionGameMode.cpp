#include "MegaregionGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "TrainPawn.h"
#include "TrainSimHUD.h"

AMegaregionGameMode::AMegaregionGameMode()
{
	// Set the HUD class to our pure C++ Slate HUD — no Blueprint dependency
	HUDClass = ATrainSimHUD::StaticClass();

	// Set the default pawn to our Train Blueprint if it exists, otherwise fallback to C++ class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_TrainPawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		// Fallback to C++ class if BP doesn't exist yet
		DefaultPawnClass = ATrainPawn::StaticClass();
	}
}
