#include "TrackGenerator.h"
#include "Components/BoxComponent.h"

ATrackGenerator::ATrackGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	// The base platform (Ties/Ballast) - 5 Kilometers long!
	CrossTies = CreateDefaultSubobject<UBoxComponent>(TEXT("CrossTies"));
	RootComponent = CrossTies;
	
	// Box extent is half-size. 250,000 units = 2.5km. Total length = 5km.
	// Width = 200 units (2 meters). Thickness = 20 units.
	CrossTies->SetBoxExtent(FVector(250000.0f, 200.0f, 20.0f)); 
	CrossTies->SetCollisionProfileName(TEXT("BlockAll"));
	CrossTies->bHiddenInGame = false; // Make sure we can see it in greybox

	// Left Rail
	LeftRail = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftRail"));
	LeftRail->SetupAttachment(RootComponent);
	LeftRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	// Standard gauge is ~143.5 cm. Place rail 71.75 cm to the left.
	LeftRail->SetRelativeLocation(FVector(0.0f, -71.75f, 35.0f));
	LeftRail->SetCollisionProfileName(TEXT("BlockAll"));
	LeftRail->bHiddenInGame = false;

	// Right Rail
	RightRail = CreateDefaultSubobject<UBoxComponent>(TEXT("RightRail"));
	RightRail->SetupAttachment(RootComponent);
	RightRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	RightRail->SetRelativeLocation(FVector(0.0f, 71.75f, 35.0f));
	RightRail->SetCollisionProfileName(TEXT("BlockAll"));
	RightRail->bHiddenInGame = false;
}
