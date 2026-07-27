#include "TrainPawn.h"
#include "Math/UnrealMathUtility.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SceneComponent.h"
#include "TrainHUDWidget.h"
#include "EconomySubsystem.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

ATrainPawn::ATrainPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Create physical root body
	UBoxComponent* LocoBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainLocoBody"));
	RootComponent = LocoBody;
	LocoBody->SetCollisionProfileName(TEXT("PhysicsActor"));
	LocoBody->SetSimulatePhysics(true);
	LocoBody->SetMassOverrideInKg(NAME_None, 10000.0f * 1000.0f, true); // 10000 tons
	LocoBody->SetBoxExtent(FVector(1000.0f, 150.0f, 200.0f)); // 20m long box

	// Lock Physics to 1D rail movement to prevent derailment
	LocoBody->BodyInstance.bLockYTranslation = true;
	LocoBody->BodyInstance.bLockXRotation = true;
	LocoBody->BodyInstance.bLockYRotation = true;
	LocoBody->BodyInstance.bLockZRotation = true;

	// Visual Mesh (Zero-Manual-Entry)
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LocoVisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeMeshAsset.Object);
		VisualMesh->SetRelativeScale3D(FVector(20.0f, 3.0f, 4.0f)); // Scale to match 20m long box
	}

	// Create Camera and Spring Arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("LocoSpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 800.0f)); // Elevate arm origin above the train
	SpringArmComp->SetRelativeRotation(FRotator(-30.0f, 45.0f, 0.0f)); // Angle it down and to the side for a cinematic view!
	SpringArmComp->TargetArmLength = 3500.0f; // Distance to camera (far enough to see 20m train and roadblock)
	SpringArmComp->bUsePawnControlRotation = false; // Keep camera fixed at this cinematic angle
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("LocoCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera doesn't rotate relative to arm

	// Rear Coupler
	RearCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LocoRearCoupler"));
	RearCoupler->SetupAttachment(RootComponent);
	RearCoupler->SetRelativeLocation(FVector(-500.0f, 0.0f, 0.0f));

	// Rear Coupler Trigger
	RearCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("LocoRearCouplerTrigger"));
	RearCouplerTrigger->SetupAttachment(RearCoupler);
	RearCouplerTrigger->SetSphereRadius(50.0f);
	RearCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RearCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainPawn::OnCouplerOverlap);

	// Default mass variables (physics now driven by LocoBody)
	MassInTons = 10000.0f;
	MaxTractiveEffort = 500000000.0f; // 500 million Newtons (Needed to overcome 10 million kg mass in UE physics!)
	
	// Default pressures in PSI (standard US freight brake setup)
	BrakePipePressure = 90.0f;
	MainReservoirPressure = 130.0f;
	BrakeCylinderPressure = 0.0f;
	
	TargetBrakePipePressure = 90.0f;
	BrakeExhaustRate = 5.0f; // Drops 5 PSI per second
	BrakeChargeRate = 3.0f;  // Charges 3 PSI per second
	MaxBrakeForce = 2500000.0f; // Massive brake shoe force in Newtons
	
	CurrentThrottleNotch = 0.0f;
	TimeSinceLastHUDUpdate = 0.0f;

	// Possess automatically for the Visual Test!
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ATrainPawn::BeginPlay()
{
	Super::BeginPlay();

	// Clear the physics log file at the start of a new run
	if (GetLocalRole() == ROLE_Authority)
	{
		FString LogPath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
		FFileHelper::SaveStringToFile(TEXT("--- NEW PHYSICS RUN (LOCOMOTIVE SPAWNED) ---\n"), *LogPath);
	}

	// Add Enhanced Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		// Initialize UI Widget
		if (HUDWidgetClass)
		{
			HUDWidgetInstance = CreateWidget<UTrainHUDWidget>(PlayerController, HUDWidgetClass);
			if (HUDWidgetInstance)
			{
				HUDWidgetInstance->AddToViewport();
			}
		}
	}

	// Phase 2.2: Generate a Contract automatically for the Visual Test!
	if (UEconomySubsystem* EconomySystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		EconomySystem->GenerateRandomContract();
	}

	// For the visual test, immediately apply 50% throttle (Notch 4) so it moves!
	SetThrottleNotch(4.0f);
}

void ATrainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	LogPhysicsState();

	// --- Pneumatic Brake Simulation (Fluid Dynamics over time) ---
	if (TargetBrakePipePressure < BrakePipePressure)
	{
		// Exhausting air to apply brakes
		BrakePipePressure -= BrakeExhaustRate * DeltaTime;
		if (BrakePipePressure < TargetBrakePipePressure) BrakePipePressure = TargetBrakePipePressure;
	}
	else if (TargetBrakePipePressure > BrakePipePressure)
	{
		// Pumping air to release brakes
		BrakePipePressure += BrakeChargeRate * DeltaTime;
		if (BrakePipePressure > TargetBrakePipePressure) BrakePipePressure = TargetBrakePipePressure;
	}

	// Calculate Brake Cylinder Pressure (1 PSI pipe drop = 2.5 PSI cylinder increase)
	// Max cylinder pressure is usually 64 PSI for a 90 PSI pipe.
	float PressureDrop = 90.0f - BrakePipePressure;
	BrakeCylinderPressure = FMath::Clamp(PressureDrop * 2.5f, 0.0f, 64.0f);

	// --- Basic throttle logic ---
	if (CurrentThrottleNotch > 0.0f)
	{
		// Calculate linear force to be applied in the physics solver
		float AppliedForce = (CurrentThrottleNotch / 8.0f) * MaxTractiveEffort; 
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
		{
			FVector ForwardVector = GetActorForwardVector();
			PrimComp->AddForce(ForwardVector * AppliedForce);
		}
	}

	// Update HUD if it exists (Throttled to 10 FPS to prevent Web Browser from hanging the engine)
	if (HUDWidgetInstance)
	{
		TimeSinceLastHUDUpdate += DeltaTime;
		if (TimeSinceLastHUDUpdate >= 0.1f)
		{
			// Fetch Economy Balance & Contract
			UEconomySubsystem* EconomySystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
			int32 Wallet = 0;
			FString ContractStr = TEXT("No Contract");
			
			if (EconomySystem)
			{
				Wallet = EconomySystem->GetPlayerBalance();
				ContractStr = EconomySystem->GetActiveContractDetails();
			}
			
			float SpeedKmh = GetVelocity().Size() * 0.036f; // cm/s to km/h
			HUDWidgetInstance->UpdateHUDMetrics(SpeedKmh, BrakePipePressure, BrakeCylinderPressure, CurrentThrottleNotch, Wallet, ContractStr);
			TimeSinceLastHUDUpdate = 0.0f;
		}
	}
}

void ATrainPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind Throttle and Brake Actions
		if (ThrottleAction)
		{
			EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATrainPawn::ThrottleInput);
		}
		
		if (BrakeAction)
		{
			EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ATrainPawn::BrakeInput);
		}
	}
}

void ATrainPawn::ThrottleInput(const FInputActionValue& Value)
{
	float ThrottleValue = Value.Get<float>();
	SetThrottleNotch(CurrentThrottleNotch + (ThrottleValue * 1.0f)); // Increment/Decrement notch
}

void ATrainPawn::BrakeInput(const FInputActionValue& Value)
{
	float BrakeValue = Value.Get<float>();
	// E.g., positive input reduces target pressure (applies brakes)
	SetTargetBrakePressure(TargetBrakePipePressure - (BrakeValue * 2.0f));
}

void ATrainPawn::SetThrottleNotch(float Notch)
{
	// Locomotives typically have 8 throttle notches
	CurrentThrottleNotch = FMath::Clamp(Notch, 0.0f, 8.0f);
}

void ATrainPawn::SetTargetBrakePressure(float TargetPressure)
{
	TargetBrakePipePressure = FMath::Clamp(TargetPressure, 0.0f, 90.0f);
}

void ATrainPawn::OnCouplerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp && OtherComp->IsSimulatingPhysics())
	{
		if (RearAttachedCar != nullptr) return;

		RearCoupler->SetConstrainedComponents(Cast<UPrimitiveComponent>(RootComponent), NAME_None, Cast<UPrimitiveComponent>(OtherComp), NAME_None);
		
		RearCoupler->SetLinearXLimit(LCM_Limited, 15.0f);
		RearCoupler->SetLinearYLimit(LCM_Locked, 0.0f);
		RearCoupler->SetLinearZLimit(LCM_Locked, 0.0f);
		
		RearCoupler->SetAngularSwing1Limit(ACM_Limited, 10.0f);
		RearCoupler->SetAngularSwing2Limit(ACM_Limited, 10.0f);
		RearCoupler->SetAngularTwistLimit(ACM_Locked, 0.0f);

		RearAttachedCar = OtherActor;
	}
}

void ATrainPawn::LogPhysicsState()
{
	FString LogPath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
	FVector Loc = GetActorLocation();
	FRotator Rot = GetActorRotation();
	FVector Vel = GetVelocity();
	FString LogLine = FString::Printf(TEXT("[Locomotive] Loc=(%f,%f,%f) Rot=(%f,%f,%f) Vel=(%f,%f,%f)\n"), Loc.X, Loc.Y, Loc.Z, Rot.Pitch, Rot.Yaw, Rot.Roll, Vel.X, Vel.Y, Vel.Z);
	FFileHelper::SaveStringToFile(LogLine, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

