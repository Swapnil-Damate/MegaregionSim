# Phase 1.4: Procedural Greybox Track Generation

I completely agree with you. The Web UI architecture is functionally wired up in the C++ backend, so we can ignore the visual white-screen glitch for now and polish it in a later UI-focused phase. 

It is time to move to the final step of Phase 1: **Greybox Testing**. We need a massive track to actually drive this train on and test our physics and brakes at high speeds!

## Goal
Create a C++ tool that allows you to click and drag a line in the Unreal Editor to instantly generate kilometers of physical train tracks, rather than placing track pieces one by one by hand.

## Proposed Changes

### [NEW] `TrackGenerator.h` and `TrackGenerator.cpp`
I will create a new C++ Actor class containing:
- **`USplineComponent`:** The mathematical curve you will draw in the editor.
- **`USplineMeshComponent` Generation:** Inside the C++ `OnConstruction` function, the code will measure the length of the spline you drew and automatically spawn hundreds of track meshes end-to-end, bending them perfectly along the curves.

## Your Required Actions (After C++ Push)
1. You will compile the C++ code.
2. You will drag the new `TrackGenerator` into your 3D world.
3. You will click the end of the spline line, hold `ALT` on your keyboard, and drag your mouse to extrude a 5-kilometer-long track through the mountains!
4. We will drop the Locomotive onto the rails, apply the Throttle, and test our momentum and brake physics!

> [!IMPORTANT]
> Because we are doing a Greybox (prototype), the track will just be simple stretched Cubes acting as the steel rails. We will replace them with high-quality 3D models in Phase 2.

Click **Proceed** if you are ready to build the procedural track generator!
