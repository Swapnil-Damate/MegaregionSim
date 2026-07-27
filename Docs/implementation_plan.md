# Phase 1.2 (Final Task): Pneumatic Brake Simulation

## Current Project Status
We are currently in **Phase 1: Pre-Production**, executing the final task of **Section 1.2: Core Physics Implementation**. We have successfully integrated the `ATrainPawn` C++ base class into the AWS Unreal Engine environment. We are now writing the mathematical logic for the brakes.

## Goal
Implement a highly realistic pneumatic air brake simulation in C++. Unlike cars, train brakes do not apply instantly. Air must slowly exhaust from a 2-mile-long pipe to push the brake cylinders against the wheels.

## User Review Required
> [!IMPORTANT]
> Please review the mathematical approach and the Risk Assessment below. Because we are dealing with physics forces on a 10,000-ton object, incorrect math can cause the Unreal Engine physics solver to crash or launch the train into space. Approve this plan if the logic meets your realism standards!

## Proposed Changes

### 1. Header Updates (`TrainPawn.h`)
We will add the necessary target and rate variables to simulate fluid dynamics (air pressure over time).
- **[MODIFY]** `Source/MegaregionSim/TrainPawn.h`
  - Add `TargetBrakePipePressure` (The pressure requested by the driver's lever).
  - Add `BrakeExhaustRate` (How fast air leaves the pipe, e.g., 5 PSI/sec).
  - Add `BrakeChargeRate` (How fast compressors pump air back into the pipe, e.g., 3 PSI/sec).
  - Add `MaxBrakeForce` (The maximum physical stopping force in Newtons).

### 2. Mathematics & Tick Logic (`TrainPawn.cpp`)
We will implement the frame-by-frame simulation inside the `Tick()` function.
- **[MODIFY]** `Source/MegaregionSim/TrainPawn.cpp`
  - **The Delay Simulation:** If `BrakePipePressure` is higher than `TargetBrakePipePressure`, slowly reduce it by `BrakeExhaustRate * DeltaTime`.
  - **The Cylinder Math:** Calculate `BrakeCylinderPressure`. In real trains, a 1 PSI drop in the brake pipe results in roughly a 2.5 PSI increase in the brake cylinders (up to a max of ~64 PSI).
  - **The Physical Force:** Multiply the `BrakeCylinderPressure` ratio by `MaxBrakeForce` and apply that force in the opposite direction of the train's velocity using Unreal's Chaos Physics API (`AddForce`).

## Risk Assessment & Mitigation

> [!WARNING]
> **Risk 1: Frame-Rate Dependency (The "Cyberpunk" Bug)**
> *Risk:* If we calculate air pressure changes per frame without accounting for time, a player with 120 FPS will stop their train 4x faster than a player with 30 FPS.
> *Mitigation:* EVERY pressure calculation and force application will strictly be multiplied by `DeltaTime` (the exact fraction of a second since the last frame). This guarantees mathematically identical stopping distances regardless of computer hardware.

> [!CAUTION]
> **Risk 2: Chaos Physics Instability (The Space Launch)**
> *Risk:* Applying a massive opposing force (e.g., 5 million Newtons of braking force) instantly to an Unreal Engine rigid body can cause the physics solver to freak out, launching the train into the sky.
> *Mitigation:* The braking force will be linearly interpolated (`FMath::FInterpTo`) and clamped. We will also check if the train's velocity is nearly zero and switch to `SetLinearVelocity(0)` to prevent the brakes from accidentally pushing the train backward.

## Verification Plan
Once I push this C++ code:
1. You will use **Live Coding** (`Ctrl + Alt + F11`) in Unreal Engine to compile it instantly.
2. We will attach a basic UI print node to the screen to watch the air pressure slowly drop in real-time.
