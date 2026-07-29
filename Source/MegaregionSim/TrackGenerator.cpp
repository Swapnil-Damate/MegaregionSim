#include "TrackGenerator.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATrackGenerator::ATrackGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	TrackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TrackCollision"));
	RootComponent = TrackCollision;
	TrackCollision->SetBoxExtent(FVector(250000.0f, 200.0f, 20.0f)); 
	TrackCollision->SetCollisionProfileName(TEXT("BlockAll"));

	CrossTiesISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CrossTiesISM"));
	CrossTiesISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TieAsset(TEXT("StaticMesh'/Game/FinalAssets/Tie_Main_1.Tie_Main_1'"));
	if (TieAsset.Succeeded()) CrossTiesISM->SetStaticMesh(TieAsset.Object);

	LeftRailISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LeftRailISM"));
	LeftRailISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LeftRailAsset(TEXT("StaticMesh'/Game/FinalAssets/Rail_Main_L.Rail_Main_L'"));
	if (LeftRailAsset.Succeeded()) LeftRailISM->SetStaticMesh(LeftRailAsset.Object);

	RightRailISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RightRailISM"));
	RightRailISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RightRailAsset(TEXT("StaticMesh'/Game/FinalAssets/Rail_Main_R.Rail_Main_R'"));
	if (RightRailAsset.Succeeded()) RightRailISM->SetStaticMesh(RightRailAsset.Object);

	MilepostISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MilepostISM"));
	MilepostISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MilepostAsset(TEXT("StaticMesh'/Game/FinalAssets/Milepost.Milepost'"));
	if (MilepostAsset.Succeeded()) MilepostISM->SetStaticMesh(MilepostAsset.Object);

	CatenaryPoleISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CatenaryPoleISM"));
	CatenaryPoleISM->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CatenaryAsset(TEXT("StaticMesh'/Game/FinalAssets/Catenary_Pole.Catenary_Pole'"));
	if (CatenaryAsset.Succeeded()) CatenaryPoleISM->SetStaticMesh(CatenaryAsset.Object);
}

void ATrackGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateTrack();
}

void ATrackGenerator::GenerateTrack()
{
	float TrackLength = 500000.0f;
	float TrackSegmentLength = 2000.0f; // Assume the user's Track_Mesh is 20 meters long

	for (float x = -250000.0f; x < 250000.0f; x += TrackSegmentLength)
	{
		FTransform TrackTransform;
		TrackTransform.SetLocation(FVector(x, 0.0f, 10.0f));
		CrossTiesISM->AddInstance(TrackTransform);
	}

	float MilepostSpacing = 1600000.0f;
	for (float x = -250000.0f; x < 250000.0f; x += MilepostSpacing)
	{
		FTransform MilepostTransform;
		MilepostTransform.SetLocation(FVector(x, 300.0f, 0.0f));
		MilepostISM->AddInstance(MilepostTransform);
	}

	float CatenarySpacing = 5000.0f;
	for (float x = -250000.0f; x < 250000.0f; x += CatenarySpacing)
	{
		FTransform CatenaryTransform;
		CatenaryTransform.SetLocation(FVector(x, -300.0f, 0.0f));
		CatenaryPoleISM->AddInstance(CatenaryTransform);
	}
}
