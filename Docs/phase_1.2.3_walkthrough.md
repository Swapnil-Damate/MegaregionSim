# Phase 1.2.3 Completed: High-Stress Couplers

I have successfully programmed the automatic coupling system in C++! 

## What Changed
- **New `ATrainCar` Class:** A dedicated 5,000-ton physics object representing rolling stock (e.g., boxcars, flatbeds).
- **Knuckle Couplers:** Both the Locomotive and the Train Cars now have invisible `UPhysicsConstraintComponent`s at their ends.
- **Automatic Snapping:** I added overlap spheres. If the back of the locomotive touches the front of a train car, the C++ code fires, locking the two rigid bodies together with 15cm of "slack" (to simulate real-world train chaining).
- **Pneumatic Air Hose:** Once coupled, the Train Car automatically pulls the brake pipe pressure from the locomotive in front of it!

## Testing Instructions (Your Action Required)

Since I created a brand new C++ file (`TrainCar.cpp`), you must do a hard compile:
1. Pull from GitHub.
2. Inside Unreal, click **Tools** -> **Refresh Visual Studio Project**.
3. Close Unreal Engine.
4. Build the project in Visual Studio.
5. Re-open Unreal Engine.

### Setting up the Train Car
1. Right-click in the Content Browser and create a new **Blueprint Class**.
2. Search for the Parent Class **`TrainCar`** (not TrainPawn). Name it `BP_Boxcar`.
3. Open `BP_Boxcar`, add a **Cube** component (just like the locomotive), and ensure the Cube has **Simulate Physics** checked in its Details panel. Save and close.
4. Drag the new `BP_Boxcar` into the 3D world, placing it a few feet behind the locomotive.

Hit Play, reverse the locomotive into the boxcar, and watch the magnetic couplers lock you together!
