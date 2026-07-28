#include "LevelCrossing.h"
#include "TrainPawn.h"
#include "Components/SceneComponent.h"

ALevelCrossing::ALevelCrossing()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(BaseMesh);
	
	// The warning trigger extends far down the track (1km)
	WarningTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("WarningTrigger"));
	WarningTrigger->SetupAttachment(RootComponent);
	WarningTrigger->SetBoxExtent(FVector(100000.0f, 500.0f, 500.0f)); // 1km long trigger
	WarningTrigger->SetCollisionProfileName(TEXT("Trigger"));
	
	WarningTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelCrossing::OnTrainEnterWarningZone);
	WarningTrigger->OnComponentEndOverlap.AddDynamic(this, &ALevelCrossing::OnTrainLeaveWarningZone);

	bIsTrainApproaching = false;
	CurrentGateAngle = 0.0f; // Up
	TargetGateAngle = 0.0f; 
}

void ALevelCrossing::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelCrossing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Animate the gate smoothly
	if (!FMath::IsNearlyEqual(CurrentGateAngle, TargetGateAngle, 0.1f))
	{
		CurrentGateAngle = FMath::FInterpTo(CurrentGateAngle, TargetGateAngle, DeltaTime, 2.0f);
		GateMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentGateAngle));
	}
}

void ALevelCrossing::OnTrainEnterWarningZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ATrainPawn::StaticClass()))
	{
		bIsTrainApproaching = true;
		TargetGateAngle = -90.0f; // Down
	}
}

void ALevelCrossing::OnTrainLeaveWarningZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ATrainPawn::StaticClass()))
	{
		bIsTrainApproaching = false;
		TargetGateAngle = 0.0f; // Up
	}
}
