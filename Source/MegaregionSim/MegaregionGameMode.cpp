#include "MegaregionGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "TrainPawn.h"

AMegaregionGameMode::AMegaregionGameMode()
{
	// Set the default pawn to our Train Blueprint to guarantee flawless camera possession on Frame 0
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
