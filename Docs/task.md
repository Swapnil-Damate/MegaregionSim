# Phase 2 Completion Tasks

- `[/]` Task 1: Economy Subsystem - Contract Generation
  - `[ ]` Add `GenerateContract` to `EconomySubsystem`
  - `[ ]` Save contract to SQLite
  - `[ ]` Update `HUD.html` and `TrainHUDWidget` to display current contract
- `[ ]` Task 2: Fluid Dynamics (Sloshing)
  - `[ ]` Add `bIsLiquidCargo` to `TrainCar`
  - `[ ]` Calculate acceleration in `Tick`
  - `[ ]` Shift CenterOfMass dynamically based on acceleration
- `[ ]` Task 3: Soft-Body Deformation (Crashing)
  - `[ ]` Bind `OnComponentHit` in `TrainCar`
  - `[ ]` Calculate impact force
  - `[ ]` Mathematically scale `VisualMesh` to squash if impact > threshold
- `[ ]` Task 4: Update `VisualTest.py` to trigger these scenarios
- `[ ]` Commit all changes to GitHub

# Phase 5 Completion Tasks

- `[x]` Task 1: Update HUD to show Digital/Analog Speed
- `[x]` Task 2: Speed Warnings (Flashing yellow/red)
- `[x]` Task 3: Branch Line Ending Warning
- `[x]` Task 4: Contract Countdown Timer
- `[x]` Task 5: GPS Minimap Skeleton
- `[x]` Task 6: Cinematic Director Camera UI hint
