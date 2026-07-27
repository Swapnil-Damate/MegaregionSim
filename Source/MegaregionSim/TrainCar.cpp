#include "TrainCar.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"
#include "TrainPawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

ATrainCar::ATrainCar()
{
	PrimaryActorTick.bCanEverTick = true;

	CarBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainFreightCarBody"));
	RootComponent = CarBody;
	CarBody->SetCollisionProfileName(TEXT("PhysicsActor"));
	CarBody->SetSimulatePhysics(true);
	CarBody->SetMassOverrideInKg(NAME_None, MassInTons * 1000.0f, true);
	CarBody->SetBoxExtent(FVector(1000.0f, 150.0f, 200.0f)); // 20m long box

	// Lock Physics to 1D rail movement to prevent derailment
	CarBody->BodyInstance.bLockYTranslation = true;
	CarBody->BodyInstance.bLockXRotation = true;
	CarBody->BodyInstance.bLockYRotation = true;
	CarBody->BodyInstance.bLockZRotation = true;

	// Phase 2.2 Fluid Dynamics
	bIsLiquidCargo = true; // Default to true so we don't need Python set_editor_property

	// Visual Mesh
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrainFreightVisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeMeshAsset.Object);
		VisualMesh->SetRelativeScale3D(FVector(20.0f, 3.0f, 4.0f));
	}
	
	CurrentCenterOfMassOffset = FVector::ZeroVector;
	LastVelocity = FVector::ZeroVector;

	// Soft-body crash deformation binding
	CarBody->OnComponentHit.AddDynamic(this, &ATrainCar::OnCarHit);

	// Constraints (Knuckles)
	FrontCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("FreightFrontCoupler"));
	FrontCoupler->SetupAttachment(RootComponent);
	FrontCoupler->SetRelativeLocation(FVector(500.0f, 0.0f, 0.0f));
	
	RearCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("FreightRearCoupler"));
	RearCoupler->SetupAttachment(RootComponent);
	RearCoupler->SetRelativeLocation(FVector(-500.0f, 0.0f, 0.0f));

	// Triggers
	FrontCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("FreightFrontCouplerTrigger"));
	FrontCouplerTrigger->SetupAttachment(FrontCoupler);
	FrontCouplerTrigger->SetSphereRadius(50.0f);
	FrontCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	FrontCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainCar::OnCouplerOverlap);

	RearCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("FreightRearCouplerTrigger"));
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

	LogPhysicsState();

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
	FVector CurrentVelocity = GetVelocity();
	FVector Acceleration = (CurrentVelocity - LastVelocity) / DeltaTime;
	LastVelocity = CurrentVelocity;

	if (bIsLiquidCargo)
	{
		float SloshAmount = FMath::Clamp(-Acceleration.X * 0.5f, -500.0f, 500.0f); 
		CurrentCenterOfMassOffset.X = FMath::FInterpTo(CurrentCenterOfMassOffset.X, SloshAmount, DeltaTime, 2.0f);
		CarBody->SetCenterOfMass(CurrentCenterOfMassOffset);
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

// Phase 2.3 Soft-Body Crash Deformation
void ATrainCar::OnCarHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float ImpactForce = NormalImpulse.Size();
	
	// If the impact is massive (e.g. over 5 Million Newtons of impulse)
	if (ImpactForce > 5000000.0f)
	{
		TArray<USceneComponent*> MeshChildren;
		RootComponent->GetChildrenComponents(true, MeshChildren);
		for (USceneComponent* Child : MeshChildren)
		{
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Child);
			if (Mesh && Mesh->GetName() == TEXT("VisualMesh"))
			{
				FVector CurrentScale = Mesh->GetRelativeScale3D();
				// Squash the X (length) and expand the Y/Z (bulge outward)
				FVector SquashedScale = CurrentScale * FVector(0.5f, 1.2f, 1.2f);
				Mesh->SetRelativeScale3D(SquashedScale);
			}
		}
	}
}

void ATrainCar::LogPhysicsState()
{
	FString PhysicsLogFilePath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
	FVector Loc = GetActorLocation();
	FRotator Rot = GetActorRotation();
	FVector Vel = GetVelocity();
	FString LogLine = FString::Printf(TEXT("[TrainCar] Loc=(%f,%f,%f) Rot=(%f,%f,%f) Vel=(%f,%f,%f)\n"), Loc.X, Loc.Y, Loc.Z, Rot.Pitch, Rot.Yaw, Rot.Roll, Vel.X, Vel.Y, Vel.Z);
	FFileHelper::SaveStringToFile(LogLine, *PhysicsLogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

