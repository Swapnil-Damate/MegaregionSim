#include "RailwaySignal.h"
#include "TrainPawn.h"
#include "UObject/ConstructorHelpers.h"

ARailwaySignal::ARailwaySignal()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	
	SignalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SignalMesh"));
	SignalMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SignalAsset(TEXT("StaticMesh'/Game/FinalAssets/Modern_LED_Signal_.Modern_LED_Signal_'"));
	if (SignalAsset.Succeeded()) SignalMesh->SetStaticMesh(SignalAsset.Object);

	// The block trigger represents the track section this signal protects (e.g. 5km long)
	BlockTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockTrigger"));
	BlockTrigger->SetupAttachment(RootComponent);
	BlockTrigger->SetBoxExtent(FVector(250000.0f, 500.0f, 500.0f)); // 5km block
	BlockTrigger->SetRelativeLocation(FVector(250000.0f, 0, 0)); // Extend forward from the signal
	BlockTrigger->SetCollisionProfileName(TEXT("Trigger"));
	
	BlockTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARailwaySignal::OnTrainEnterBlock);
	BlockTrigger->OnComponentEndOverlap.AddDynamic(this, &ARailwaySignal::OnTrainLeaveBlock);

	CurrentState = ESignalState::Clear;
}

void ARailwaySignal::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisuals();
}

void ARailwaySignal::OnTrainEnterBlock(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ATrainPawn::StaticClass()))
	{
		SetSignalState(ESignalState::Stop);
	}
}

void ARailwaySignal::OnTrainLeaveBlock(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ATrainPawn::StaticClass()))
	{
		SetSignalState(ESignalState::Clear);
	}
}

void ARailwaySignal::SetSignalState(ESignalState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		UpdateVisuals();
	}
}

void ARailwaySignal::UpdateVisuals()
{
	// In a real AAA game, we'd use Dynamic Material Instances to change emissive colors.
	// For now, we assume the Blueprint or C++ creates a Dynamic Material and sets the color.
	if (UMaterialInstanceDynamic* DynMat = SignalMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		FLinearColor Color = (CurrentState == ESignalState::Clear) ? FLinearColor::Green : FLinearColor::Red;
		DynMat->SetVectorParameterValue(TEXT("LightColor"), Color);
	}
}
