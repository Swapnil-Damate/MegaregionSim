# Phase 1 & 2 Visual UI Testing Plan

You want hard visual proof inside the Unreal Engine UI that the physics, pneumatics, and economy systems we wrote actually work, rather than just trusting the C++ compiler. That is completely fair. I will build an automated visual test suite.

## Objective
Automatically spawn a fully functional UI HUD on the screen that tracks live physics telemetry (Speed, Brake Pressure, Economy Wallet) while spawning two trains and physically coupling them together.

## 1. UI Blueprint Automation
- I will update our Python pipeline to automatically create a `BP_TrainHUD` based on the `TrainHUDWidget` C++ class we wrote earlier. 
- This UI uses Unreal's WebBrowser widget to load our `HUD.html` file, which displays beautiful live telemetry gauges.

## 2. Real-Time Telemetry Link
- I will modify `TrainPawn.cpp` so that when you hit "Play" in the editor, it automatically instances the HUD and displays it on your screen.
- The `Tick` function will continuously send the Physics Engine data (Velocity, Pipe Pressure, Cylinder Pressure) and the SQLite Economy data (Player Balance) directly to the UI gauges.

## 3. The Visual "Test Script" (Python)
I will write a new automation script (`VisualTest.py`) that will:
1. Spawn the Player's Locomotive (`BP_TrainPawn`).
2. Spawn a Freight Car (`BP_TrainCar`) exactly 10 meters behind it.
3. Because they are close, the Physics Engine will immediately detect the overlap and snap them together using the `UPhysicsConstraintComponent` (Coupler) we wrote.
4. When you hit Play, you will see the UI update live, the locomotive move, and the freight car get pulled via the physical slack joint.

## Zero Manual Entry
You will not need to manually create the UI Blueprints, link them, or place the trains. The Python script will handle the setup, and all you will have to do is hit **Play**.

> [!IMPORTANT]
> Since we are dealing with UI (UMG), the engine might need to briefly compile shaders when you hit Play.

Click **Proceed** if you approve this Visual Testing Suite, and I will write the Python script and C++ hooks immediately!
