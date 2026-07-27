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
#include "Components/WidgetComponent.h"
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
	
	// CRITICAL: Disable Controller Rotation overrides! If true (default), the PlayerController teleports the pawn's rotation every frame, which instantly freezes and breaks the Chaos physics solver!
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Create physical root body
	UBoxComponent* LocoBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainLocoBody"));
	RootComponent = LocoBody;
	LocoBody->SetCollisionProfileName(TEXT("PhysicsActor"));
	LocoBody->SetSimulatePhysics(true);
	LocoBody->SetMassOverrideInKg(NAME_None, 10000.0f, true); // 10 tons (Scaled down to prevent Chaos float precision errors)
	LocoBody->SetBoxExtent(FVector(1000.0f, 150.0f, 200.0f)); // 20m long box

	// Lock Physics to 1D rail movement to prevent derailment
	LocoBody->BodyInstance.bLockYTranslation = true;
	LocoBody->BodyInstance.bLockXRotation = true;
	LocoBody->BodyInstance.bLockYRotation = true;
	LocoBody->BodyInstance.bLockZRotation = true;

	// Visual Mesh (Final AAA Asset)
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LocoVisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/FinalAssets/Diesel_Locomotive.Diesel_Locomotive'"));
	if (MeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(MeshAsset.Object);
		// Note: The FBX scale should inherently be correct now, so reset the massive proxy scale to 1.0f
		VisualMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); 
		VisualMesh->SetCollisionProfileName(TEXT("NoCollision")); // Critical: Prevent complex compound shape snagging
	}

	// Setup UI Component
	HUDWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HUDWidget"));
	HUDWidgetComponent->SetupAttachment(RootComponent);
	HUDWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	
	// Setup Phase 4 Proxy Components
	AcousticsComponent = CreateDefaultSubobject<UTrainAcousticsComponent>(TEXT("AcousticsComponent"));
	EraVFXManager = CreateDefaultSubobject<UDynamicEraVFXManager>(TEXT("EraVFXManager"));

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
	MassInTons = 10.0f;
	MaxTractiveEffort = 5000000.0f; // 5 Million Newtons (Scaled down to prevent Chaos solver failure, but strong enough to rocket forward)
	
	// Default pressures in PSI (standard US freight brake setup)
	BrakePipePressure = 90.0f;
	MainReservoirPressure = 130.0f;
	BrakeCylinderPressure = 0.0f;
	
	TargetBrakePipePressure = 90.0f;
	BrakeExhaustRate = 5.0f; // Drops 5 PSI per second
	BrakeChargeRate = 3.0f;  // Charges 3 PSI per second
	MaxBrakeForce = 25000.0f; // Scaled down brake force
	
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
		FString PhysicsLogFilePath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
		FFileHelper::SaveStringToFile(TEXT("--- NEW PHYSICS RUN (LOCOMOTIVE SPAWNED) ---\n"), *PhysicsLogFilePath);
	}

	// Initialize Air Brake System
	MainReservoirPressure = 130.0f; // 130 psi
	BrakePipePressure = 90.0f;		// 90 psi (Fully released)
	BrakeCylinderPressure = 0.0f;	// 0 psi
	TargetBrakePipePressure = 90.0f;

	// Note: Camera possession is now perfectly handled natively by MegaregionGameMode.
	
	// Initialize UI Widget automatically using the pure C++ class
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		HUDWidgetInstance = CreateWidget<UTrainHUDWidget>(PlayerController, UTrainHUDWidget::StaticClass());
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			HUDWidgetInstance->SetTrainPawn(this);
		}
	}

	// Phase 5: Spawn the 8-car Freight Consist automatically
	SpawnConsist();

	// Phase 2.2: Generate a Contract automatically for the Visual Test!
	if (UEconomySubsystem* EconomySystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		EconomySystem->GenerateRandomContract();
	}

	// For the visual test, immediately apply 100% throttle (Notch 8) so it rockets down the track!
	SetThrottleNotch(8.0f);
}

void ATrainPawn::SpawnConsist()
{
	if (GetLocalRole() != ROLE_Authority) return;

	UClass* ContainerClass = FindObject<UClass>(ANY_PACKAGE, TEXT("/Script/Engine.StaticMeshActor"));
	if (!ContainerClass) return;

	UStaticMesh* ContainerMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/FinalAssets/FreightContainer.FreightContainer"));
	
	FVector SpawnLoc = GetActorLocation();
	FVector ForwardVec = GetActorForwardVector();
	
	AActor* LastCar = this;

	for (int i = 0; i < 8; i++)
	{
		// Spawn cars 2000 units behind each other
		SpawnLoc -= (ForwardVec * 2000.0f);
		
		FActorSpawnParameters SpawnParams;
		AActor* NewCar = GetWorld()->SpawnActor<AActor>(ContainerClass, SpawnLoc, GetActorRotation(), SpawnParams);
		
		if (NewCar)
		{
			UStaticMeshComponent* MeshComp = NewCar->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComp && ContainerMesh)
			{
				MeshComp->SetStaticMesh(ContainerMesh);
				MeshComp->SetSimulatePhysics(true);
				MeshComp->SetMassOverrideInKg(NAME_None, 5000.0f, true); // 5 tons each
			}
			
			// In a full implementation, we'd spawn a UPhysicsConstraintComponent to couple them.
			// For this prototype, spawning them on the track is sufficient to demonstrate the consist.
		}
	}
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
	
	// Phase 4 Integration: Feed physics data into Audio and VFX Managers
	float SpeedMetersPerSecond = GetVelocity().Size() * 0.01f;
	
	if (AcousticsComponent)
	{
		// Proxy Engine RPM calculation (Speed * Throttle)
		float EngineRPM = 800.0f + (SpeedMetersPerSecond * CurrentThrottleNotch * 5.0f);
		AcousticsComponent->UpdateEngineAcoustics(EngineRPM, SpeedMetersPerSecond);
	}

	if (EraVFXManager)
	{
		float EngineLoad = CurrentThrottleNotch / 8.0f;
		EraVFXManager->UpdateVFXState(SpeedMetersPerSecond, EngineLoad);
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
	FString PhysicsLogFilePath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
	FVector Loc = GetActorLocation();
	FRotator Rot = GetActorRotation();
	FVector Vel = GetVelocity();
	FString LogLine = FString::Printf(TEXT("[Locomotive] Loc=(%f,%f,%f) Rot=(%f,%f,%f) Vel=(%f,%f,%f)\n"), Loc.X, Loc.Y, Loc.Z, Rot.Pitch, Rot.Yaw, Rot.Roll, Vel.X, Vel.Y, Vel.Z);
	FFileHelper::SaveStringToFile(LogLine, *PhysicsLogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

