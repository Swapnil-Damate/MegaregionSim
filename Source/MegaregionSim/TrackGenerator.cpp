#include "TrackGenerator.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATrackGenerator::ATrackGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	// Load the final AAA Track Mesh FBX for visuals
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackMeshAsset(TEXT("StaticMesh'/Game/FinalAssets/Track_Mesh.Track_Mesh'"));
	UStaticMesh* TrackMesh = TrackMeshAsset.Object;

	// The base platform (Ties/Ballast) - 5 Kilometers long!
	CrossTies = CreateDefaultSubobject<UBoxComponent>(TEXT("TrackCrossTies"));
	RootComponent = CrossTies;
	
	// Box extent is half-size. 250,000 units = 2.5km. Total length = 5km.
	// Width = 200 units (2 meters). Thickness = 20 units.
	CrossTies->SetBoxExtent(FVector(250000.0f, 200.0f, 20.0f)); 
	CrossTies->SetCollisionProfileName(TEXT("BlockAll"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackBaseMesh"));
	BaseMesh->SetupAttachment(CrossTies);
	if (TrackMesh) BaseMesh->SetStaticMesh(TrackMesh);
	// Reset the 5000x proxy scale since the FBX will be correctly sized by PCG later, or we scale the FBX natively.
	// For now, we will leave the relative scale at 1.0, 1.0, 1.0 (assuming the FBX is a 5km long mesh, or we rely on spline meshes in Phase 5)
	BaseMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); 
	BaseMesh->SetCollisionProfileName(TEXT("NoCollision")); // Prevent physics interference

	// Left Rail
	LeftRail = CreateDefaultSubobject<UBoxComponent>(TEXT("TrackLeftRail"));
	LeftRail->SetupAttachment(RootComponent);
	LeftRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	// Standard gauge is ~143.5 cm. Place rail 71.75 cm to the left.
	LeftRail->SetRelativeLocation(FVector(0.0f, -71.75f, 35.0f));
	LeftRail->SetCollisionProfileName(TEXT("NoCollision")); // Prevent physics glitches from flat boxes resting on thin edges

	LeftRailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackLeftRailMesh"));
	LeftRailMesh->SetupAttachment(LeftRail);
	// We no longer need separate visual rails because the FBX includes them!
	LeftRailMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// Right Rail
	RightRail = CreateDefaultSubobject<UBoxComponent>(TEXT("TrackRightRail"));
	RightRail->SetupAttachment(RootComponent);
	RightRail->SetBoxExtent(FVector(250000.0f, 10.0f, 15.0f));
	RightRail->SetRelativeLocation(FVector(0.0f, 71.75f, 35.0f));
	RightRail->SetCollisionProfileName(TEXT("NoCollision")); // Prevent physics glitches from flat boxes resting on thin edges

	RightRailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackRightRailMesh"));
	RightRailMesh->SetupAttachment(RightRail);
	// We no longer need separate visual rails because the FBX includes them!
	RightRailMesh->SetCollisionProfileName(TEXT("NoCollision"));
}
