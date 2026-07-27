# Phase 1.2.3 Implementation Plan: High-Stress Couplers

## Goal
Fulfill the missed Phase 1.2.3 requirement by programming the physical Couplers. We need to allow multiple rolling stock vehicles to physically link to the Locomotive, pull 10,000-ton loads using Chaos Physics, and share pneumatic brake pipe pressure across the entire consist.

## Proposed C++ Architecture

### 1. Rolling Stock Base Class (`ATrainCar`)
- **[NEW]** I will write a new C++ class called `ATrainCar`. This will serve as the base class for all non-locomotive rolling stock (boxcars, flatbeds, etc.).
- Like the locomotive, this class will have extreme Mass properties (e.g., 5,000 tons) and its own localized brake cylinders.

### 2. The Knuckle Couplers (`UPhysicsConstraintComponent`)
- **[MODIFY]** I will add front and rear `UPhysicsConstraintComponent`s to both `ATrainPawn` (Locomotive) and `ATrainCar`.
- These constraints will act as the physical steel knuckles. I will configure their limits to allow a tiny amount of "slack action" (which causes that classic crashing sound when a train starts moving) but lock them from separating entirely.

### 3. Brake Pipe Integration
- **[MODIFY]** When two cars successfully couple, they will share a pointer to each other.
- During the `Tick()` function, the brake pipe pressure will equalize between connected cars (simulating the air hoses connecting). If the locomotive applies the brakes, the pressure drop will ripple down the train cars, causing them to apply their own brake cylinders.

## Risk Assessment

> [!CAUTION]
> **Risk 1: Physics Engine Explosion**
> *Issue:* Linking two 10,000-ton Chaos Physics rigid bodies together with a 0-distance constraint can cause extreme mathematical instability, making the train instantly fly into space.
> *Mitigation:* I will enable "Soft Constraints" with high stiffness/damping, which allows the physics engine slightly more mathematical flexibility without tearing the cars apart.

## User Action Required
Once I push the C++ code, you will need to:
1. Pull the code and Live Compile (`Ctrl + Alt + F11`).
2. Create a Blueprint out of `ATrainCar` (just like you did for the locomotive).
3. Place a few of them behind the locomotive in the 3D world so we can test the coupling!

## Open Questions for the Director
1. **Coupler Engagement:** Should coupling be entirely automatic (if the train bumps into a car, they instantly lock together), or do you want the player to have to press a button to lock the couplers once they are touching?
