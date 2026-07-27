# Revised Phase 1 & 2 Master Verification Plan

You provided excellent feedback. You successfully saw the two trains and the track *before* hitting Play, but everything broke apart *after* hitting Play. 

## Diagnosing the Visual Failures
I reviewed our architecture and immediately found exactly why it broke when you hit Play:
1. **The "Exploding Box" Bug:** Our `AutomatedTrainSpawner` C++ subsystem was hardcoded to automatically spawn a train at `(0,0,200)` the moment you hit Play. Because our Python script *also* spawned trains there, the two trains overlapped inside each other. The Chaos Physics Engine violently rejected the overlap and shot one of the boxes off the track at Mach 10!
2. **The "Missing WebUI" Bug:** The Python script successfully attached the UI to the Editor trains, but because you didn't *possess* the train (you were just a flying camera), Unreal Engine didn't know which screen to attach the HTML UI to.

## The Fix: Comprehensive UI & Physics Proofing
We will update our C++ and Python pipeline to definitively prove all requirements from Phase 1 and Phase 2.

### 1. Physics & Coupling (Phase 1.2 & 1.4)
- I will delete the hardcoded C++ train spawner logic so the Python trains don't explode.
- I will update `VisualTest.py` to force your `PlayerController` to **Possess** the Locomotive the moment you hit Play. You will be sitting inside the locomotive.

### 2. UI & Web Telemetry (Phase 1.3)
- Because you are now Possessing the train, the `TrainPawn` will correctly spawn the `BP_TrainHUD` and glue it to your screen. You will see the Live Speed, Air Brakes, and SQLite Economy numbers.

### 3. Economy & AI (Phase 2.1 & 2.2)
- I will add a small Python script command that forces the train's throttle to 50% when the level starts. 
- You will physically see the Locomotive pull the Freight car (proving coupling).
- You will see the Speed and Air Brake numbers dynamically change on the UI (proving physics math).
- You will see the SQLite Wallet Balance fetch from the database and display on screen (proving Phase 2.2).

Click **Proceed** to approve this revised verification plan, and I will deploy these fixes immediately!
