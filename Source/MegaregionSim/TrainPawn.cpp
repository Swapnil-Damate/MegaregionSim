// ─────────────────────────────────────────────────────────────────────────────
// TrainPawn.cpp — Complete definitive rewrite of movement and consist systems
// ─────────────────────────────────────────────────────────────────────────────
#include "TrainPawn.h"
#include "MegaregionGameMode.h"
#include "TrainCar.h"
#include "MegaregionWeatherSystem.h"
#include "TrainSimHUD.h"
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
#include "Components/SplineComponent.h"
#include "MegaregionZoningGenerator.h"
#include "OpenWorldGraphGenerator.h"

ATrainPawn::ATrainPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Disable controller rotation overrides — these fight kinematic movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;
	
	// ── Physical root ──────────────────────────────────────────────────────────
	UBoxComponent* LocoBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainLocoBody"));
	RootComponent = LocoBody;
	LocoBody->SetCollisionProfileName(TEXT("PhysicsActor"));
	// IMPORTANT: physics DISABLED — we drive the train kinematically so it always
	// moves exactly where the player commands.  Physics is re-enabled only for
	// crash impulses in DerailTrain().
	LocoBody->SetSimulatePhysics(false);
	LocoBody->SetBoxExtent(FVector(1000.0f, 150.0f, 150.0f)); // 20m long, 3m wide, 3m tall

	// ── Train visual mesh ──────────────────────────────────────────────────────
	UStaticMeshComponent* TrainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrainMesh"));
	TrainMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrainAsset(TEXT("StaticMesh'/Game/FinalAssets/Diesel_Locomotive.Diesel_Locomotive'"));
	if (TrainAsset.Succeeded())
	{
		TrainMesh->SetStaticMesh(TrainAsset.Object);
	}
	else
	{
		// Fallback: use a basic cube so the train is always visible
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
		if (CubeAsset.Succeeded())
		{
			TrainMesh->SetStaticMesh(CubeAsset.Object);
			TrainMesh->SetRelativeScale3D(FVector(20.0f, 3.0f, 4.0f)); // 20m × 3m × 4m loco shape
		}
	}
	TrainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TrainMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// ── Acoustics & VFX ───────────────────────────────────────────────────────
	AcousticsComponent = CreateDefaultSubobject<UTrainAcousticsComponent>(TEXT("AcousticsComponent"));
	EraVFXManager       = CreateDefaultSubobject<UDynamicEraVFXManager>(TEXT("EraVFXManager"));

	// ── Spring arm & camera ───────────────────────────────────────────────────
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("LocoSpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 600.0f));
	SpringArmComp->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));
	SpringArmComp->TargetArmLength = 4500.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bDoCollisionTest = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("LocoCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// ── Headlight ─────────────────────────────────────────────────────────────
	Headlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Headlight"));
	Headlight->SetupAttachment(RootComponent);
	Headlight->SetRelativeLocation(FVector(1000.0f, 0.0f, 350.0f));
	Headlight->VolumetricScatteringIntensity = 1.0f;
	Headlight->AttenuationRadius = 25000.0f;
	Headlight->bUseTemperature = true;
	Headlight->Temperature = 4000.0f;

	// ── Rear coupler (physical coupling trigger) ───────────────────────────────
	RearCoupler = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LocoRearCoupler"));
	RearCoupler->SetupAttachment(RootComponent);
	RearCoupler->SetRelativeLocation(FVector(-500.0f, 0.0f, 0.0f));

	RearCouplerTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("LocoRearCouplerTrigger"));
	RearCouplerTrigger->SetupAttachment(RearCoupler);
	RearCouplerTrigger->SetSphereRadius(50.0f);
	RearCouplerTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RearCouplerTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrainPawn::OnCouplerOverlap);

	// ── Catenary sparks ───────────────────────────────────────────────────────
	CatenarySparks = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CatenarySparks"));
	CatenarySparks->SetupAttachment(RootComponent);
	CatenarySparks->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	CatenarySparks->SetAutoActivate(true);

	// ── Train physics defaults ─────────────────────────────────────────────────
	MassInTons        = 10.0f;
	MaxTractiveEffort = 5000000.0f; // 5 MN (used to derive max speed)

	BrakePipePressure       = 90.0f;
	MainReservoirPressure   = 130.0f;
	BrakeCylinderPressure   = 0.0f;
	TargetBrakePipePressure = 90.0f;
	BrakeExhaustRate        = 5.0f;
	BrakeChargeRate         = 3.0f;
	MaxBrakeForce           = 8000000.0f;
	DynamicBrakeEffort      = 0.0f;
	ContinuousBrakeTime     = 0.0f;

	CurrentThrottleNotch = 0.0f;
	CurrentThrust        = 0.0f;
	CurrentSpeedMs       = 0.0f;
	CurrentDistanceAlongSpline = 20000.0f;
	MainTrackSplineRef   = nullptr;
	TimeSinceLastHUDUpdate = 0.0f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// MUST be false so the camera can orbit 360 degrees instead of spinning the entire train
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// ─────────────────────────────────────────────────────────────────────────────
void ATrainPawn::BeginPlay()
{
	Super::BeginPlay();

	// Explicitly disable physics simulation on the root component to guarantee
	// that serialized Blueprint properties do NOT override our kinematic movement.
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		RootPrim->SetSimulatePhysics(false);
		RootPrim->SetCollisionProfileName(TEXT("OverlapAll")); // Don't collide or block
	}

	// Find the World Generator and cache its spline
	AActor* GeneratorActor = UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorldGraphGenerator::StaticClass());
	if (GeneratorActor)
	{
		AOpenWorldGraphGenerator* Generator = Cast<AOpenWorldGraphGenerator>(GeneratorActor);
		if (Generator && Generator->ExpressTrackForward)
		{
			MainTrackSplineRef = Generator->ExpressTrackForward;
		}
	}

	// Initialize starting location
	if (GetActorLocation().X > 25000.0f)
	{
		CurrentDistanceAlongSpline = GetActorLocation().X; // AI trains spawned far ahead
	}
	else
	{
		CurrentDistanceAlongSpline = 20000.0f; // Player train
	}

	if (MainTrackSplineRef)
	{
		FVector StartLoc = MainTrackSplineRef->GetLocationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator StartRot = MainTrackSplineRef->GetRotationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		
		if (bOnParallelTrack)
		{
			// Shift AI trains to the parallel track
			FVector RightVec = FRotationMatrix(StartRot).GetScaledAxis(EAxis::Y);
			StartLoc += (RightVec * 3500.0f);
		}
		
		StartLoc.Z += 100.0f; // sit exactly on top of rails
		SetActorLocationAndRotation(StartLoc, StartRot);
	}
	else
	{
		SetActorLocationAndRotation(FVector(CurrentDistanceAlongSpline, 0.0f, 130.0f), FRotator::ZeroRotator);
	}

	// Clear physics log
	if (GetLocalRole() == ROLE_Authority)
	{
		FString PhysicsLogFilePath = FPaths::ProjectSavedDir() / TEXT("PhysicsDebugLog.txt");
		FFileHelper::SaveStringToFile(TEXT("--- NEW RUN ---\n"), *PhysicsLogFilePath);
	}

	// Air brake initialisation
	MainReservoirPressure   = 130.0f;
	BrakePipePressure       = 90.0f;
	BrakeCylinderPressure   = 0.0f;
	TargetBrakePipePressure = 90.0f;

	// HUD is pure C++ — data is pushed every tick via ATrainSimHUD::UpdateData()

	// Spawn consist after 0.5s — let the world generator finish spawning all chunks first
	FTimerHandle ConsistSpawnTimer;
	GetWorldTimerManager().SetTimer(ConsistSpawnTimer, this, &ATrainPawn::SpawnConsist, 0.5f, false);

	// Generate starting contract
	if (UEconomySubsystem* EconomySystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
	{
		EconomySystem->GenerateRandomContract();
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// SpawnConsist — kinematic consist, no physics constraints
// Each car is placed at a fixed offset and then tracked in ConsistCars for
// per-tick kinematic following.
// ─────────────────────────────────────────────────────────────────────────────
void ATrainPawn::SpawnConsist()
{
    if (GetLocalRole() != ROLE_Authority) return;
    
    // Wait for start menu to be completed
    AMegaregionGameMode* GM = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode());
    if (GM && !GM->bStartMenuComplete)
    {
        // Re-check in 0.5s
        FTimerHandle RetryTimer;
        GetWorldTimerManager().SetTimer(RetryTimer, this, &ATrainPawn::SpawnConsist, 0.5f, false);
        return;
    }
    
    int32 NumCars = (GM) ? GM->SelectedCarCount : 8;
    int32 CarType = (GM) ? GM->SelectedCarType : 3; // 3 = Mixed
    int32 EngineIdx = (GM) ? GM->SelectedEngineIndex : 0;
    
    // Load the selected locomotive mesh
    UStaticMeshComponent* TrainMesh = FindComponentByClass<UStaticMeshComponent>();
    if (TrainMesh)
    {
        UStaticMesh* EngMesh = nullptr;
        switch (EngineIdx)
        {
        case 0: EngMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FinalAssets/Diesel_Locomotive.Diesel_Locomotive'")); break;
        case 1: EngMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FinalAssets/Steam_Locomotive.Steam_Locomotive'")); break;
        case 2: EngMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FinalAssets/Bullet_Train.Bullet_Train'")); break;
        case 3: EngMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FinalAssets/Urban_Metro_Tram.Urban_Metro_Tram'")); break;
        }
        if (EngMesh)
        {
            TrainMesh->SetStaticMesh(EngMesh);
            TrainMesh->SetRelativeScale3D(FVector(1.0f));
        }
    }
    
    ConsistCars.Empty();
    
    const float CarSpacing = 2600.0f; // 26m per car slot (prevents clipping of long cars)
    FVector ForwardVec = GetActorForwardVector();
    FRotator LocoRot = GetActorRotation();
    
    for (int32 i = 0; i < NumCars; i++)
    {
        FVector SpawnLoc;
        FRotator SpawnRot;
        
        if (MainTrackSplineRef)
        {
            float CarDist = CurrentDistanceAlongSpline - CarSpacing * (i + 1);
            SpawnLoc = MainTrackSplineRef->GetLocationAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
            SpawnRot = MainTrackSplineRef->GetRotationAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
            SpawnLoc.Z += 100.0f;
            
            // Apply parallel track offset if needed
            if (bOnParallelTrack)
            {
                FVector Right = MainTrackSplineRef->GetRightVectorAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
                SpawnLoc += Right * ParallelTrackOffset;
            }
        }
        else
        {
            SpawnLoc = GetActorLocation() - ForwardVec * CarSpacing * (i + 1);
            SpawnLoc.Z = GetActorLocation().Z;
            SpawnRot = LocoRot;
        }
        
        FActorSpawnParameters SP;
        SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        ATrainCar* NewCar = GetWorld()->SpawnActor<ATrainCar>(ATrainCar::StaticClass(), SpawnLoc, SpawnRot, SP);
        if (NewCar)
        {
            NewCar->CarBody->SetSimulatePhysics(false);
            NewCar->CarBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            
            // Configure car visuals based on selection
            if (CarType == 3) // Mixed
            {
                NewCar->ConfigureCarVisuals(i); // Uses i % 3 for variety
            }
            else
            {
                NewCar->ConfigureCarVisuals(CarType); // All same type
            }
            
            NewCar->FrontAttachedCar = (ConsistCars.Num() > 0)
                ? (AActor*)ConsistCars.Last()
                : (AActor*)this;
            
            ConsistCars.Add(NewCar);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void ATrainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ── Pneumatic brake sim ────────────────────────────────────────────────────
	if (TargetBrakePipePressure < BrakePipePressure)
	{
		BrakePipePressure -= BrakeExhaustRate * DeltaTime;
		BrakePipePressure  = FMath::Max(BrakePipePressure, TargetBrakePipePressure);
	}
	else if (TargetBrakePipePressure > BrakePipePressure)
	{
		BrakePipePressure += BrakeChargeRate * DeltaTime;
		BrakePipePressure  = FMath::Min(BrakePipePressure, TargetBrakePipePressure);
	}

	float PressureDrop    = 90.0f - BrakePipePressure;
	BrakeCylinderPressure = FMath::Clamp(PressureDrop * 2.5f, 0.0f, 64.0f);

	// ── Keyboard input (W / S / Space) ────────────────────────────────────────
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC->IsInputKeyDown(EKeys::W))
			SetThrottleNotch(CurrentThrottleNotch + (3.0f * DeltaTime));
		else if (PC->IsInputKeyDown(EKeys::S))
			SetThrottleNotch(CurrentThrottleNotch - (3.0f * DeltaTime));

		if (PC->IsInputKeyDown(EKeys::SpaceBar))
			SetTargetBrakePressure(FMath::Max(0.0f, TargetBrakePipePressure - 15.0f * DeltaTime));
		else if (!PC->IsInputKeyDown(EKeys::SpaceBar) && TargetBrakePipePressure < 90.0f)
			SetTargetBrakePressure(FMath::Min(90.0f, TargetBrakePipePressure + 5.0f * DeltaTime));
			
		if (PC->WasInputKeyJustPressed(EKeys::H))
			PlayHorn();
	}

	// ── KINEMATIC SPEED MODEL ─────────────────────────────────────────────────
	// Target speed is proportional to throttle notch.  Max speed = 250 km/h.
	const float MaxSpeedMs  = 250.0f / 3.6f; // 69.4 m/s
	float TargetSpeedMs     = (CurrentThrottleNotch / 8.0f) * MaxSpeedMs;

	// Brakes reduce target speed
	float BrakeRatio = BrakeCylinderPressure / 64.0f;

	// Overhaul Air Brakes: simulate brake overheating if brakes held continuously > 60 seconds while moving
	if (BrakeRatio > 0.01f && CurrentSpeedMs > 1.0f)
	{
		ContinuousBrakeTime += DeltaTime;
		if (ContinuousBrakeTime > 60.0f)
		{
			BrakeRatio = 0.0f;
			UE_LOG(LogTemp, Warning, TEXT("Brakes Overheated! Braking effort failed."));
		}
	}
	else
	{
		ContinuousBrakeTime = 0.0f;
	}

	if (BrakeRatio > 0.01f)
	{
		float MaxBrakeDecelMs = 5.0f; // 5 m/s² max deceleration
		TargetSpeedMs = FMath::Max(0.0f, TargetSpeedMs - BrakeRatio * MaxBrakeDecelMs);
	}

	// Smooth acceleration — 0→100 km/h in ~10 seconds
	float AccelRate = (TargetSpeedMs > CurrentSpeedMs) ? 3.0f : 6.0f; // faster braking
	
	// Cargo Weight Physics: Heavily dampen acceleration if pulling > 10 cars
	if (ConsistCars.Num() > 10 && TargetSpeedMs > CurrentSpeedMs)
	{
		AccelRate *= 0.1f;
	}
	
	CurrentSpeedMs  = FMath::FInterpConstantTo(CurrentSpeedMs, TargetSpeedMs, DeltaTime, AccelRate);

	// Wind Resistance: Math cap where aerodynamic drag forcefully clamps max speed at 250 km/h
	const float MaxAbsoluteSpeedMs = 250.0f / 3.6f;
	if (CurrentSpeedMs > MaxAbsoluteSpeedMs)
	{
		CurrentSpeedMs = MaxAbsoluteSpeedMs;
	}

	// Wheel Slip: True if throttle is maxed while raining
	bIsWheelSlipping = false;
	if (CurrentThrottleNotch >= 8.0f)
	{
		AMegaregionWeatherSystem* WeatherSys = Cast<AMegaregionWeatherSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AMegaregionWeatherSystem::StaticClass()));
		if (WeatherSys && WeatherSys->CurrentWeather == EWeatherState::Rain)
		{
			bIsWheelSlipping = true;
		}
	}

	// Coupler Slack: Clank debug logs when starting from 0
	if (bWasStopped && CurrentSpeedMs > 0.1f)
	{
		CouplerSlackClanksRemaining = 12;
		CouplerSlackTimer = 0.5f;
	}
	bWasStopped = (CurrentSpeedMs <= 0.1f);
	
	if (CouplerSlackClanksRemaining > 0)
	{
		CouplerSlackTimer -= DeltaTime;
		if (CouplerSlackTimer <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("CLANK! (Coupler Slack)"));
			CouplerSlackClanksRemaining--;
			CouplerSlackTimer = 0.5f;
		}
	}

	// ── Move loco and consist kinematically ─────────────────────────────────────
	if (MainTrackSplineRef)
	{
		// Auto-Routing: Switch automatically at junctions
		if (bAutoRouteEnabled)
		{
			AMegaregionGameMode* GM = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				float FrameDist = CurrentSpeedMs * 100.0f * DeltaTime;
				for (float TurnoutDist : GM->TurnoutDistances)
				{
					float DistToTurnout = TurnoutDist - CurrentDistanceAlongSpline;
					if (DistToTurnout <= 0.0f && DistToTurnout > -FrameDist)
					{
						AOpenWorldGraphGenerator* Generator = Cast<AOpenWorldGraphGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorldGraphGenerator::StaticClass()));
						if (Generator)
						{
							if (MainTrackSplineRef == Generator->ExpressTrackForward) MainTrackSplineRef = Generator->FreightTrackForward;
							else if (MainTrackSplineRef == Generator->FreightTrackForward) MainTrackSplineRef = Generator->ExpressTrackReverse;
							else if (MainTrackSplineRef == Generator->ExpressTrackReverse) MainTrackSplineRef = Generator->FreightTrackReverse;
							else if (MainTrackSplineRef == Generator->FreightTrackReverse) MainTrackSplineRef = Generator->ExpressTrackForward;
							UE_LOG(LogTemp, Warning, TEXT("Auto-Route: Switched automatically at junction."));
						}
					}
				}
			}
		}

		CurrentDistanceAlongSpline += CurrentSpeedMs * 100.0f * DeltaTime;

		// Issue 19: Derail if train falls off the end of the track to prevent car stacking
		if (CurrentDistanceAlongSpline >= MainTrackSplineRef->GetSplineLength() - 100.0f)
		{
			DerailTrain();
			return;
		}

		FVector NewLoc = MainTrackSplineRef->GetLocationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator NewRot = MainTrackSplineRef->GetRotationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		
		// Apply parallel track lateral offset
		if (bOnParallelTrack)
		{
		    FVector RightVec = MainTrackSplineRef->GetRightVectorAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		    NewLoc += RightVec * ParallelTrackOffset;
		}

		NewLoc.Z += 20.0f; // Offset to sit on tracks
		
		SetActorLocationAndRotation(NewLoc, NewRot);

		// ── Follow consist kinematically ──────────────────────────────────────────
		const float CarSpacing = 2200.0f; // 22m per car slot (Issue 3: Increase distance)
		for (int i = 0; i < ConsistCars.Num(); i++)
		{
			if (ConsistCars[i] && IsValid(ConsistCars[i]))
			{
				float CarDist = CurrentDistanceAlongSpline - CarSpacing * (i + 1);
				FVector CarLoc = MainTrackSplineRef->GetLocationAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
				FRotator CarRot = MainTrackSplineRef->GetRotationAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
				
				if (bOnParallelTrack)
				{
				    FVector CarRight = MainTrackSplineRef->GetRightVectorAtDistanceAlongSpline(CarDist, ESplineCoordinateSpace::World);
				    CarLoc += CarRight * ParallelTrackOffset;
				}
				
				CarLoc.Z += 20.0f; // Issue 3: Drop cars to 20cm so wheels touch rails
				ConsistCars[i]->SetActorLocationAndRotation(CarLoc, CarRot);
			}
		}
	}
	else
	{
		// Fallback: move along forward vector
		FVector ForwardVec = GetActorForwardVector();
		FVector NewLoc     = GetActorLocation() + ForwardVec * (CurrentSpeedMs * 100.0f * DeltaTime);
		NewLoc.Z = 20.0f;
		SetActorLocation(NewLoc);

		// ── Follow consist kinematically ──────────────────────────────────────────
		const float CarSpacing = 2200.0f; // 22m per car slot (Issue 3: Increase distance)
		for (int i = 0; i < ConsistCars.Num(); i++)
		{
			if (ConsistCars[i] && IsValid(ConsistCars[i]))
			{
				FVector CarLoc = GetActorLocation() - ForwardVec * CarSpacing * (i + 1);
				CarLoc.Z = 20.0f; // Issue 3
				ConsistCars[i]->SetActorLocationAndRotation(CarLoc, GetActorRotation());
			}
		}
	}

	// ── VFX / Acoustics ───────────────────────────────────────────────────────
	float SpeedMetersPerSecond = CurrentSpeedMs;
	if (AcousticsComponent)
	{
		float EngineRPM = 800.0f + (SpeedMetersPerSecond * CurrentThrottleNotch * 5.0f);
		AcousticsComponent->UpdateEngineAcoustics(EngineRPM, SpeedMetersPerSecond);
	}
	if (EraVFXManager)
	{
		float EngineLoad = CurrentThrottleNotch / 8.0f;
		if (CurrentSpeedMs * 3.6f > 300.0f)
			DerailTrain();
	}

	// ── Push data to Slate HUD ────────────────────────────────────────────────
	TimeSinceLastHUDUpdate += DeltaTime;
	if (TimeSinceLastHUDUpdate >= 0.1f)
	{
		TimeSinceLastHUDUpdate = 0.0f;
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (ATrainSimHUD* SimHUD = Cast<ATrainSimHUD>(PC->GetHUD()))
			{
				UEconomySubsystem* Eco = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
				int32 Wallet = Eco ? Eco->GetPlayerBalance() : 0;
				bool  bLight = Headlight ? Headlight->IsVisible() : false;
				FString Signal = TEXT("GREEN");

				FHitResult Hit;
				FVector Ts = GetActorLocation();
				FVector Te = Ts + GetActorForwardVector() * 20000.0f;
				FCollisionQueryParams QP;
				QP.AddIgnoredActor(this);
				if (GetWorld()->LineTraceSingleByChannel(Hit, Ts, Te, ECC_Visibility, QP))
					if (Hit.GetActor() && Hit.GetActor()->GetName().Contains(TEXT("Signal")))
						Signal = TEXT("RED");

				SimHUD->UpdateData(CurrentSpeedMs * 3.6f, CurrentThrottleNotch,
					BrakePipePressure, BrakeCylinderPressure, Wallet, Signal, bLight);
			}
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
void ATrainPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ThrottleAction)
			EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATrainPawn::ThrottleInput);
		if (BrakeAction)
			EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ATrainPawn::BrakeInput);
		if (SwitchTrainAction)
			EnhancedInputComponent->BindAction(SwitchTrainAction, ETriggerEvent::Started, this, &ATrainPawn::SwitchTrainInput);
	}

	PlayerInputComponent->BindKey(EKeys::L, IE_Pressed, this, &ATrainPawn::ToggleHeadlight);
	PlayerInputComponent->BindKey(EKeys::H, IE_Pressed, this, &ATrainPawn::PlayHorn);
	PlayerInputComponent->BindKey(EKeys::J, IE_Pressed, this, &ATrainPawn::SwitchTrack);
	PlayerInputComponent->BindKey(EKeys::V, IE_Pressed, this, &ATrainPawn::ToggleCinematicCamera);

	// 360° camera orbit
	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &APawn::AddControllerPitchInput);
}

void ATrainPawn::SwitchTrainInput(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->UnPossess();
		FActorSpawnParameters SP;
		SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 1500.0f);
		if (AActor* DroneCam = GetWorld()->SpawnActor<AActor>(ADefaultPawn::StaticClass(), SpawnLoc, GetActorRotation(), SP))
			if (APawn* DronePawn = Cast<APawn>(DroneCam))
				PC->Possess(DronePawn);
	}
}

void ATrainPawn::ToggleCinematicCamera()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		PC = GetWorld()->GetFirstPlayerController();
	}

	if (PC)
	{
		if (PC->GetPawn() == this)
		{
			PC->UnPossess();
			if (!CinematicDroneCam)
			{
				FActorSpawnParameters SP;
				SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				FVector SpawnLoc = GetActorLocation() + FVector(-1500.0f, 1500.0f, 1000.0f);
				FRotator SpawnRot = (GetActorLocation() - SpawnLoc).Rotation();
				CinematicDroneCam = GetWorld()->SpawnActor<ADefaultPawn>(ADefaultPawn::StaticClass(), SpawnLoc, SpawnRot, SP);
			}
			else
			{
				CinematicDroneCam->SetActorLocation(GetActorLocation() + FVector(-1500.0f, 1500.0f, 1000.0f));
				CinematicDroneCam->SetActorRotation((GetActorLocation() - CinematicDroneCam->GetActorLocation()).Rotation());
			}
			PC->Possess(CinematicDroneCam);
			EnableInput(PC);
		}
		else if (PC->GetPawn() == CinematicDroneCam)
		{
			PC->UnPossess();
			PC->Possess(this);
			DisableInput(PC);
		}
	}
}

void ATrainPawn::ThrottleInput(const FInputActionValue& Value)
{
	SetThrottleNotch(CurrentThrottleNotch + Value.Get<float>());
}

void ATrainPawn::BrakeInput(const FInputActionValue& Value)
{
	SetTargetBrakePressure(TargetBrakePipePressure - Value.Get<float>() * 2.0f);
}

void ATrainPawn::SetThrottleNotch(float NewNotch)
{
	if (GetLocalRole() < ROLE_Authority) Server_SetThrottleNotch(NewNotch);
	CurrentThrottleNotch = FMath::Clamp(NewNotch, 0.0f, 8.0f);
}
void ATrainPawn::Server_SetThrottleNotch_Implementation(float NewNotch) { SetThrottleNotch(NewNotch); }
bool ATrainPawn::Server_SetThrottleNotch_Validate(float NewNotch) { return true; }

void ATrainPawn::SetTargetBrakePressure(float NewPressure)
{
	if (GetLocalRole() < ROLE_Authority) Server_SetTargetBrakePressure(NewPressure);
	TargetBrakePipePressure = FMath::Clamp(NewPressure, 0.0f, 90.0f);
}
void ATrainPawn::Server_SetTargetBrakePressure_Implementation(float P) { SetTargetBrakePressure(P); }
bool ATrainPawn::Server_SetTargetBrakePressure_Validate(float P) { return true; }

void ATrainPawn::OnCouplerOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	// Kinematic consist — overlap coupling is a no-op
}

void ATrainPawn::LogPhysicsState()
{
	// Throttled to reduce disk I/O — called only via timer if needed
}

void ATrainPawn::ToggleHeadlight()
{
	if (Headlight) Headlight->SetVisibility(!Headlight->IsVisible());
}

void ATrainPawn::PlayHorn()
{
	UE_LOG(LogTemp, Log, TEXT("HORN BLAST! Speed=%.1f km/h Throttle=%.0f"), CurrentSpeedMs * 3.6f, CurrentThrottleNotch);
	
	// Fallback to an engine notification sound so the user definitively hears it
	USoundBase* HornSound = LoadObject<USoundBase>(nullptr, TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
	if (HornSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HornSound, GetActorLocation());
	}
}

void ATrainPawn::SwitchTrack()
{
    if (GetLocalRole() < ROLE_Authority) { Server_SwitchTrack(); return; }
    
    // Auto-Routing: mathematically lock J key
    if (bAutoRouteEnabled)
    {
        UE_LOG(LogTemp, Warning, TEXT("Manual switch locked (Auto-Routing enabled)."));
        return;
    }

    if (!MainTrackSplineRef) return;
    
    // Find nearest turnout ahead
    AMegaregionGameMode* GM = Cast<AMegaregionGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;
    
    bool bFoundTurnout = false;
    for (float TurnoutDist : GM->TurnoutDistances)
    {
        float DistToTurnout = TurnoutDist - CurrentDistanceAlongSpline;
        if (DistToTurnout > 0.0f && DistToTurnout < 20000.0f) // Within 200m ahead
        {
            bFoundTurnout = true;
            break;
        }
    }
    
    if (bFoundTurnout)
    {
        // Physically transition to next spline
        AOpenWorldGraphGenerator* Generator = Cast<AOpenWorldGraphGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorldGraphGenerator::StaticClass()));
        if (Generator)
        {
             if (MainTrackSplineRef == Generator->ExpressTrackForward) MainTrackSplineRef = Generator->FreightTrackForward;
             else if (MainTrackSplineRef == Generator->FreightTrackForward) MainTrackSplineRef = Generator->ExpressTrackReverse;
             else if (MainTrackSplineRef == Generator->ExpressTrackReverse) MainTrackSplineRef = Generator->FreightTrackReverse;
             else if (MainTrackSplineRef == Generator->FreightTrackReverse) MainTrackSplineRef = Generator->ExpressTrackForward;
        }
        UE_LOG(LogTemp, Warning, TEXT("Track switch! Physically transitioned to next spline."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No turnout within 200m ahead. Cannot switch."));
    }
}
void ATrainPawn::Server_SwitchTrack_Implementation() { SwitchTrack(); }
bool ATrainPawn::Server_SwitchTrack_Validate() { return true; }

void ATrainPawn::DerailTrain()
{
	UE_LOG(LogTemp, Error, TEXT("DERAIL: speed limit exceeded. Reducing to safe speed."));
	CurrentSpeedMs = 0.0f;
	SetThrottleNotch(0.0f);
}
