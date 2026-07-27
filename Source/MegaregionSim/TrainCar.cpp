#include "TrainCar.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"
#include "TrainPawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATrainCar::ATrainCar()
{
	PrimaryActorTick.bCanEverTick = true;

	CarBody = CreateDefaultSubobject<UBoxComponent>(TEXT("CarBody"));
	RootComponent = CarBody;
	CarBody->SetSimulatePhysics(true);
	CarBody->SetMassOverrideInKg(NAME_None, MassInTons * 1000.0f, true);
	CarBody->SetBoxExtent(FVector(1000.0f, 150.0f, 200.0f)); // 20m long box

	// Visual Mesh
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeMeshAsset.Object);
		VisualMesh->SetRelativeScale3D(FVector(20.0f, 3.0f, 4.0f));
	}
	
	LastVelocity = FVector::ZeroVector;

	// Constraints (Knuckles)
	FrontCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("FrontCoupler"));
	FrontCoupler->SetupAttachment(RootComponent);
	FrontCoupler->SetRelativeLocation(FVector(500.0f, 0.0f, 0.0f));
	
	RearCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RearCoupler"));
	RearCoupler->SetupAttachment(RootComponent);
	RearCoupler->SetRelativeLocation(FVector(-500.0f, 0.0f, 0.0f));

	// Triggers
	FrontCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("FrontCouplerTrigger"));
	FrontCouplerTrigger->SetupAttachment(FrontCoupler);
	FrontCouplerTrigger->SetSphereRadius(50.0f);
	FrontCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	FrontCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainCar::OnCouplerOverlap);

	RearCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("RearCouplerTrigger"));
	RearCouplerTrigger->SetupAttachment(RearCoupler);
	RearCouplerTrigger->SetSphereRadius(50.0f);
	RearCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RearCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainCar::OnCouplerOverlap);

	BrakePipePressure = 90.0f; // Ambient pressure
}

void ATrainCar::BeginPlay()
{
	Super::BeginPlay();
}

void ATrainCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Pneumatic Brake Line Sharing
	if (FrontAttachedCar)
	{
		if (ATrainCar* FrontCar = Cast<ATrainCar>(FrontAttachedCar))
		{
			BrakePipePressure = FMath::FInterpTo(BrakePipePressure, FrontCar->BrakePipePressure, DeltaTime, 5.0f);
		}
		else if (ATrainPawn* Loco = Cast<ATrainPawn>(FrontAttachedCar))
		{
			BrakePipePressure = FMath::FInterpTo(BrakePipePressure, Loco->BrakePipePressure, DeltaTime, 5.0f);
		}
	}

	// Calculate Dynamic Sloshing (Fluid shifting forward when braking hard)
	FVector CurrentVelocity = CarBody->GetPhysicsLinearVelocity();
	FVector Acceleration = (CurrentVelocity - LastVelocity) / DeltaTime;
	LastVelocity = CurrentVelocity;

	// If we are decelerating rapidly (e.g. braking), shift center of mass forward
	if (Acceleration.X < -100.0f) 
	{
		// Shift Center of Mass dynamically
		CarBody->SetCenterOfMass(FVector(100.0f, 0, 0));
	}
	else if (Acceleration.X > 100.0f)
	{
		CarBody->SetCenterOfMass(FVector(-100.0f, 0, 0));
	}
	else
	{
		// Restore normal COM
		CarBody->SetCenterOfMass(FVector(0, 0, 0));
	}

	// Calculate localized cylinder pressure based on pipe drop
	float PressureDrop = 90.0f - BrakePipePressure;
	BrakeCylinderPressure = FMath::Clamp(PressureDrop * 2.5f, 0.0f, 64.0f);
}

void ATrainCar::OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp && OtherComp->IsSimulatingPhysics())
	{
		// Only couple if we aren't already coupled on this side
		if (OverlappedComponent == FrontCouplerTrigger && FrontAttachedCar != nullptr) return;
		if (OverlappedComponent == RearCouplerTrigger && RearAttachedCar != nullptr) return;

		// Perform the physical connection
		UPhysicsConstraintComponent* ActiveCoupler = (OverlappedComponent == FrontCouplerTrigger) ? FrontCoupler : RearCoupler;
		
		ActiveCoupler->SetConstrainedComponents(CarBody, NAME_None, Cast<UPrimitiveComponent>(OtherComp), NAME_None);
		
		// Setup high-stress Slack mechanics
		ActiveCoupler->SetLinearXLimit(LCM_Limited, 15.0f); // 15cm of slack
		ActiveCoupler->SetLinearYLimit(LCM_Locked, 0.0f);
		ActiveCoupler->SetLinearZLimit(LCM_Locked, 0.0f);
		
		ActiveCoupler->SetAngularSwing1Limit(ACM_Limited, 10.0f); // 10 degrees curve flex
		ActiveCoupler->SetAngularSwing2Limit(ACM_Limited, 10.0f);
		ActiveCoupler->SetAngularTwistLimit(ACM_Locked, 0.0f);

		if (OverlappedComponent == FrontCouplerTrigger)
		{
			FrontAttachedCar = OtherActor;
		}
		else
		{
			RearAttachedCar = OtherActor;
		}
	}
}
