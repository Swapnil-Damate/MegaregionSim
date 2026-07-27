#include "TrackGenerator.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATrackGenerator::ATrackGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	// Load the default Engine Cube mesh to use for visuals
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	UStaticMesh* CubeMesh = CubeMeshAsset.Object;

	// The base platform (Ties/Ballast) - 5 Kilometers long!
	CrossTies = CreateDefaultSubobject<UBoxComponent>(TEXT("CrossTies"));
	RootComponent = CrossTies;
	
	// Box extent is half-size. 250,000 units = 2.5km. Total length = 5km.
	// Width = 200 units (2 meters). Thickness = 20 units.
	CrossTies->SetBoxExtent(FVector(250000.0f, 200.0f, 20.0f)); 
	CrossTies->SetCollisionProfileName(TEXT("BlockAll"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(CrossTies);
	if (CubeMesh) BaseMesh->SetStaticMesh(CubeMesh);
	BaseMesh->SetRelativeScale3D(FVector(5000.0f, 4.0f, 0.4f)); // Scale the 100x100x100 cube to match the 500,000x400x40 box

	// Left Rail
	LeftRail = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftRail"));
	LeftRail->SetupAttachment(RootComponent);
	LeftRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	// Standard gauge is ~143.5 cm. Place rail 71.75 cm to the left.
	LeftRail->SetRelativeLocation(FVector(0.0f, -71.75f, 35.0f));
	LeftRail->SetCollisionProfileName(TEXT("BlockAll"));

	LeftRailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRailMesh"));
	LeftRailMesh->SetupAttachment(LeftRail);
	if (CubeMesh) LeftRailMesh->SetStaticMesh(CubeMesh);
	LeftRailMesh->SetRelativeScale3D(FVector(5000.0f, 0.2f, 0.3f));

	// Right Rail
	RightRail = CreateDefaultSubobject<UBoxComponent>(TEXT("RightRail"));
	RightRail->SetupAttachment(RootComponent);
	RightRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	RightRail->SetRelativeLocation(FVector(0.0f, 71.75f, 35.0f));
	RightRail->SetCollisionProfileName(TEXT("BlockAll"));

	RightRailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRailMesh"));
	RightRailMesh->SetupAttachment(RightRail);
	if (CubeMesh) RightRailMesh->SetStaticMesh(CubeMesh);
	RightRailMesh->SetRelativeScale3D(FVector(5000.0f, 0.2f, 0.3f));
}
