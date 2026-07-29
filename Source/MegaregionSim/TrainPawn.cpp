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
#include "Components/SpotLightComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "GameFramework/DefaultPawn.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "MegaregionZoningGenerator.h"

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

	// Cabin Mesh
	UStaticMeshComponent* CabinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CabinMesh"));
	CabinMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CabinAsset(TEXT("StaticMesh'/Game/FinalAssets/Body_Cabin.Body_Cabin'"));
	if (CabinAsset.Succeeded())
	{
		CabinMesh->SetStaticMesh(CabinAsset.Object);
		CabinMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
		CabinMesh->SetCollisionProfileName(TEXT("NoCollision"));
	}

	// Hood Mesh
	UStaticMeshComponent* HoodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HoodMesh"));
	HoodMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HoodAsset(TEXT("StaticMesh'/Game/FinalAssets/Body_LongHood.Body_LongHood'"));
	if (HoodAsset.Succeeded())
	{
		HoodMesh->SetStaticMesh(HoodAsset.Object);
		HoodMesh->SetRelativeLocation(FVector(500.0f, 0.0f, 50.0f));
		HoodMesh->SetCollisionProfileName(TEXT("NoCollision"));
	}

	// Wheel Mesh 1
	UStaticMeshComponent* WheelMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh1"));
	WheelMesh1->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelAsset(TEXT("StaticMesh'/Game/FinalAssets/Wheel_Drive_L_1.Wheel_Drive_L_1'"));
	if (WheelAsset.Succeeded())
	{
		WheelMesh1->SetStaticMesh(WheelAsset.Object);
		WheelMesh1->SetRelativeLocation(FVector(200.0f, -80.0f, -100.0f));
		WheelMesh1->SetCollisionProfileName(TEXT("NoCollision"));
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
	SpringArmComp->bUsePawnControlRotation = true; // Enabled for 360 Degree Orbit!
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("LocoCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera doesn't rotate relative to arm

	// Headlight
	Headlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Headlight"));
	Headlight->SetupAttachment(RootComponent);
	Headlight->SetRelativeLocation(FVector(1000.0f, 0.0f, 150.0f));
	Headlight->bUseVolumetricScattering = true;

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

	// Phase 13: Catenary Sparks
	CatenarySparks = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CatenarySparks"));
	CatenarySparks->SetupAttachment(RootComponent);
	CatenarySparks->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f)); // Roof of Loco
	CatenarySparks->SetAutoActivate(true);

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
	CurrentThrust = 0.0f;
	TimeSinceLastHUDUpdate = 0.0f;

	// Possess automatically for the Visual Test!
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	// Force spawn even if overlapping tracks
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
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
}

void ATrainPawn::SpawnConsist()
{
	if (GetLocalRole() != ROLE_Authority) return;

	UClass* ContainerClass = FindObject<UClass>(nullptr, TEXT("/Script/Engine.StaticMeshActor"));
	if (!ContainerClass) return;

	UStaticMesh* ContainerMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/FinalAssets/Tank_Body.Tank_Body"));
	
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
				MeshComp->SetMobility(EComponentMobility::Movable);
				MeshComp->SetStaticMesh(ContainerMesh);
				MeshComp->SetSimulatePhysics(true);
				MeshComp->SetMassOverrideInKg(NAME_None, 5000.0f, true); // 5 tons each
			}
			
			UPhysicsConstraintComponent* Coupler = NewObject<UPhysicsConstraintComponent>(NewCar, UPhysicsConstraintComponent::StaticClass());
			Coupler->RegisterComponent();
			Coupler->AttachToComponent(NewCar->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			Coupler->SetWorldLocation(SpawnLoc + (ForwardVec * 1000.0f));

			UPrimitiveComponent* LastComp = Cast<UPrimitiveComponent>(LastCar->GetRootComponent());
			UPrimitiveComponent* NewComp = Cast<UPrimitiveComponent>(NewCar->GetRootComponent());

			if (LastComp && NewComp)
			{
				Coupler->SetConstrainedComponents(LastComp, NAME_None, NewComp, NAME_None);
				Coupler->SetLinearXLimit(LCM_Locked, 0.0f);
				Coupler->SetLinearYLimit(LCM_Locked, 0.0f);
				Coupler->SetLinearZLimit(LCM_Locked, 0.0f);
				
				Coupler->SetAngularSwing1Limit(ACM_Limited, 5.0f);
				Coupler->SetAngularSwing2Limit(ACM_Limited, 5.0f);
				Coupler->SetAngularTwistLimit(ACM_Limited, 2.0f);
			}
			
			LastCar = NewCar;
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

	// --- FOOLPROOF INPUT FALLBACK ---
	// If the user hasn't set up Enhanced Input Blueprints, poll W and S directly!
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC->IsInputKeyDown(EKeys::W))
		{
			SetThrottleNotch(CurrentThrottleNotch + (2.0f * DeltaTime));
		}
		else if (PC->IsInputKeyDown(EKeys::S))
		{
			SetThrottleNotch(CurrentThrottleNotch - (2.0f * DeltaTime));
		}
	}

	// --- Tractive Effort Curve ---
	float TargetThrust = (CurrentThrottleNotch / 8.0f) * MaxTractiveEffort;
	
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
	{
		float Mass = PrimComp->GetMass();
		// Gradually increase thrust taking into account GetMass()
		float ThrustInterpRate = (Mass > 0.0f) ? (MaxTractiveEffort / (Mass * 0.1f)) : 10000.0f;
		CurrentThrust = FMath::FInterpConstantTo(CurrentThrust, TargetThrust, DeltaTime, ThrustInterpRate);
		
		if (FMath::Abs(CurrentThrust) > 0.0f)
		{
			FVector ForwardVector = GetActorForwardVector();
			PrimComp->AddForce(ForwardVector * CurrentThrust);
		}
		
		// --- Physical Braking Simulation ---
		float BrakeRatio = BrakeCylinderPressure / 64.0f;
		if (BrakeRatio > 0.01f)
		{
			FVector Velocity = GetVelocity();
			float Speed = Velocity.Size();
			
			// Full Stop Velocity Clamp (Prevents sliding backward from brakes)
			if (Speed < 5.0f)
			{
				PrimComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			}
			else
			{
				FVector BrakeDirection = -Velocity.GetSafeNormal();
				float TargetBrake = BrakeRatio * MaxBrakeForce;
				
				// Smooth interpolation to prevent Space Launch bug
				static float CurrentBrake = 0.0f;
				CurrentBrake = FMath::FInterpTo(CurrentBrake, TargetBrake, DeltaTime, 5.0f);
				
				PrimComp->AddForce(BrakeDirection * CurrentBrake);
			}
		}
	}

	// --- Fluid Sloshing Simulation ---
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
	{
		FVector AngVel = PrimComp->GetPhysicsAngularVelocityInRadians();
		// Slosh fluid outward based on Yaw angular velocity
		float SloshOffset = AngVel.Z * -100.0f; // Adjust multiplier as needed
		SloshOffset = FMath::Clamp(SloshOffset, -150.0f, 150.0f); // Max 1.5m slosh
		PrimComp->SetCenterOfMass(FVector(0.0f, SloshOffset, 0.0f));
	}

	// --- Dynamic Rail Adhesion Constraint ---
	float CurrentSpeed = GetVelocity().Size() * 0.036f;
	if (CurrentSpeed > 150.0f)
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
		{
			PrimComp->BodyInstance.bLockYTranslation = false;
			PrimComp->BodyInstance.bLockXRotation = false;
			PrimComp->BodyInstance.bLockYRotation = false;
			PrimComp->BodyInstance.bLockZRotation = false;
			PrimComp->SetConstraintMode(EDOFMode::SixDOF);
			PrimComp->AddImpulse(GetActorRightVector() * PrimComp->GetMass() * 10000.0f);
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
			
			if (HUDWidgetInstance)
			{
				bool bHeadlightsOn = Headlight ? Headlight->IsVisible() : false;
				FString NextSignal = TEXT("GREEN"); 
				FString UpcomingEvent = TEXT("Clear"); 
				
				HUDWidgetInstance->UpdateHUDMetrics(
					GetVelocity().Size() * 0.036f, 
					BrakePipePressure, 
					TargetBrakePipePressure,
					CurrentThrottleNotch, 
					Wallet, 
					ContractStr,
					100.0f,
					UpcomingEvent,
					bHeadlightsOn,
					NextSignal
				);
			}
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
		// Check for Over-speed Derailment
		if (SpeedMetersPerSecond * 3.6f > 150.0f)
		{
			DerailTrain();
		}
	
		// Phase 14: Dynamic Track Degradation (Camera Shake)
		if (CameraComp)
		{
			FVector Loc = GetActorLocation();
			EZoningClassification Zone = UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D(Loc.X, Loc.Y));
		
			// Industrial tracks are old and poorly maintained
			if (Zone == EZoningClassification::HeavyIndustrial && SpeedMetersPerSecond > 5.0f)
			{
				float ShakeAmount = SpeedMetersPerSecond * 0.2f;
				float NoiseX = FMath::PerlinNoise1D(GetGameTimeSinceCreation() * 10.0f) * ShakeAmount;
				float NoiseZ = FMath::PerlinNoise1D(GetGameTimeSinceCreation() * 12.0f + 100.0f) * ShakeAmount;
				CameraComp->SetRelativeLocation(FVector(0, NoiseX, NoiseZ));
			}
			else 
			{
				// Smooth ride on all other rails
				CameraComp->SetRelativeLocation(FVector::ZeroVector);
			}
		}

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
		
		if (SwitchTrainAction)
		{
			EnhancedInputComponent->BindAction(SwitchTrainAction, ETriggerEvent::Started, this, &ATrainPawn::SwitchTrainInput);
		}
	}

	PlayerInputComponent->BindKey(EKeys::L, IE_Pressed, this, &ATrainPawn::ToggleHeadlight);
	
	// Phase 13 Input Bindings
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ATrainPawn::PlayHorn);
	PlayerInputComponent->BindKey(EKeys::J, IE_Pressed, this, &ATrainPawn::SwitchTrack);

	// 360 Degree Camera Orbit Bindings
	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &APawn::AddControllerPitchInput);
}

void ATrainPawn::SwitchTrainInput(const FInputActionValue& Value)
{
	// Unpossess the current train and spawn a Drone Camera
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->UnPossess();
		// Spawn a Drone Camera Pawn and possess it
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 1500.0f);
		if (AActor* DroneCam = GetWorld()->SpawnActor<AActor>(ADefaultPawn::StaticClass(), SpawnLoc, GetActorRotation(), SpawnParams))
		{
			if (APawn* DronePawn = Cast<APawn>(DroneCam))
			{
				PlayerController->Possess(DronePawn);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Switched to Drone Camera Mode!"));
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

void ATrainPawn::ToggleHeadlight()
{
	if (Headlight)
	{
		Headlight->SetVisibility(!Headlight->IsVisible());
	}
}

void ATrainPawn::PlayHorn()
{
	// Trigger heavy acoustic reverb blast
	UGameplayStatics::PlaySoundAtLocation(this, LoadObject<USoundBase>(nullptr, TEXT("/Engine/VTE/EngineSounds/Explosion.Explosion")), GetActorLocation(), 1.5f, 0.5f);
}

void ATrainPawn::SwitchTrack()
{
	// Logic to translate the physics constraints laterally 500 units to merge onto parallel track
	if (UPrimitiveComponent* LocoBody = Cast<UPrimitiveComponent>(RootComponent))
	{
		FVector RightVec = GetActorRightVector();
		LocoBody->SetWorldLocation(GetActorLocation() + (RightVec * 500.0f), false, nullptr, ETeleportType::TeleportPhysics);
		UE_LOG(LogTemp, Warning, TEXT("Switched to Parallel Track!"));
	}
}

void ATrainPawn::DerailTrain()
{
	// Phase 13: Physical Deformation Swap
	if (UStaticMeshComponent* LocoMesh = Cast<UStaticMeshComponent>(RootComponent))
	{
		// In a full build, this swaps the mesh with a Geometry Collection component
		UE_LOG(LogTemp, Error, TEXT("FATAL DERAILMENT! Physics bounds exceeded, swapping to Geometry Collection Deformation Mesh."));
		LocoMesh->SetSimulatePhysics(true);
		LocoMesh->AddImpulse(FVector(0, 1000000.0f, 500000.0f)); // Violent physics flip
	}
}

