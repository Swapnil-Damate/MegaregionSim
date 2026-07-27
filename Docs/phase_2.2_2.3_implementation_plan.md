# Phase 2.2 & 2.3 Implementation Plan

This is a massive update that will permanently establish the persistence (save data) and physical realism of the Megaregion, followed by a fully automated CI/CD testing pipeline so you never have to manually test if the game is broken!

## Phase 2.2: Economy & Save Systems (SQLite)
*Goal: Persistent money, reputation, and freight contracts without manual saving.*

**Proposed Changes:**
1. **Enable SQLite:** I will programmatically enable Unreal Engine's built-in `SQLiteCore` and `SQLiteSupport` plugins in your `.uproject` and `Build.cs` files.
2. **`UEconomySubsystem.cpp`:** I will write a C++ `UGameInstanceSubsystem` that automatically runs the moment the game boots. It will look for a `MegaregionDB.sqlite` file on your hard drive. If it doesn't exist, it will automatically create it and generate SQL tables for `PlayerWallet`, `CompanyReputation`, and `ActiveContracts`. 

## Phase 2.3: Advanced Dynamic Physics
*Goal: Fluid sloshing and cargo weight shifting.*

**Proposed Changes:**
1. **`ATrainCar::Tick` Update:** I will inject dynamic mass-shifting physics. When the train brakes hard, the code will calculate the negative acceleration and mathematically shift the Center of Mass of the `ATrainCar` forward, simulating thousands of gallons of fluid sloshing to the front of a tanker car. This will dynamically affect your braking distance!

## The Automated Testing Script (CI/CD Pipeline)
*Goal: 100% Automated testing of the C++ and Physics.*

**Proposed Changes:**
1. **`MegaregionTests.cpp`:** I will write native C++ Unreal Engine Automation Tests using the `IMPLEMENT_SIMPLE_AUTOMATION_TEST` macro. It will programmatically spawn a train, apply the brakes, and assert that the physics equations return the correct values.
2. **`RunTests.bat`:** I will write a Windows Batch script that you can double-click. It will boot Unreal Engine in "Headless Mode" (no graphics rendering, pure CPU), run all the AI and Physics tests at lightning speed, and spit out a `TestResults.log` file telling us if everything passed!

> [!IMPORTANT]
> The automated test script will require the absolute path to your Unreal Engine 5 installation folder on your AWS server (e.g., `C:\Program Files\Epic Games\UE_5.x\Engine\Binaries\Win64\UnrealEditor-Cmd.exe`). If you know this path, please provide it! Otherwise, I will write the script so you can easily paste the path in yourself.

Click **Proceed** if you approve this massive automation and physics update!
