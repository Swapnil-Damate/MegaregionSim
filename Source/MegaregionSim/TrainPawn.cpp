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

ATrainPawn::ATrainPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Create physical root body
	UBoxComponent* LocoBody = CreateDefaultSubobject<UBoxComponent>(TEXT("LocoBody"));
	RootComponent = LocoBody;
	LocoBody->SetSimulatePhysics(true);
	LocoBody->SetMassOverrideInKg(NAME_None, 10000.0f * 1000.0f, true); // 10000 tons
	LocoBody->SetBoxExtent(FVector(1000.0f, 150.0f, 200.0f)); // 20m long box

	// Visual Mesh (Zero-Manual-Entry)
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeMeshAsset.Object);
		VisualMesh->SetRelativeScale3D(FVector(20.0f, 3.0f, 4.0f)); // Scale to match 20m long box
	}

	// Create Camera and Spring Arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f; // Distance to camera
	SpringArmComp->bUsePawnControlRotation = true; // Rotate arm based on controller
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera doesn't rotate relative to arm

	// Rear Coupler
	RearCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RearCoupler"));
	RearCoupler->SetupAttachment(RootComponent);
	RearCoupler->SetRelativeLocation(FVector(-500.0f, 0.0f, 0.0f));

	// Rear Coupler Trigger
	RearCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("RearCouplerTrigger"));
	RearCouplerTrigger->SetupAttachment(RearCoupler);
	RearCouplerTrigger->SetSphereRadius(50.0f);
	RearCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RearCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainPawn::OnCouplerOverlap);

	// Default mass variables (physics now driven by LocoBody)
	MassInTons = 10000.0f;
	MaxTractiveEffort = 500000.0f; // Newtons
	
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

	// For the visual test, immediately apply 50% throttle (Notch 4) so it moves!
	SetThrottleNotch(4.0f);
}

void ATrainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
			PrimComp->AddForce(ForwardVector * AppliedForce);
		}
	}

	// Update HUD if it exists (Throttled to 10 FPS to prevent Web Browser from hanging the engine)
	if (HUDWidgetInstance)
	{
		TimeSinceLastHUDUpdate += DeltaTime;
		if (TimeSinceLastHUDUpdate >= 0.1f)
		{
			// Fetch Economy Balance
			UEconomySubsystem* EconomySystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
			int32 Wallet = EconomySystem ? EconomySystem->GetPlayerBalance() : 0;
			
			float SpeedKmh = GetVelocity().Size() * 0.036f; // cm/s to km/h
			HUDWidgetInstance->UpdateHUDMetrics(SpeedKmh, BrakePipePressure, BrakeCylinderPressure, CurrentThrottleNotch, Wallet);
			TimeSinceLastHUDUpdate = 0.0f;
		}
	}
}

void ATrainPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
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
