# Phase 2.1 Implementation Plan: The Railroad System

## Goal
Now that the core physics, UI, and camera are complete, we are moving into **Phase 2: Environment & World Building**. The first step is to lay down the railroad tracks and program the train to actually drive on them, instead of driving freely like a car.

## Proposed C++ Architecture
Because you are new to Unreal Engine, I will handle 100% of the heavy math. I am going to build a **Spline-Based Rail System** in C++.

### 1. The Track Generator (`ATrackSpline`)
- **[NEW]** I will write a new C++ class called `ATrackSpline`. 
- This will allow you to drag a line across your 3D mountains, and the C++ code will automatically generate perfect railroad tracks along that line. You will simply click and drag points in the editor to shape the curves.

### 2. The Magnetic Wheels (`TrainPawn.cpp`)
- **[MODIFY]** I will update our existing `TrainPawn` C++ physics engine. 
- Right now, the train can drive anywhere. I will add a "Magnetic Snap" algorithm. Every 1/60th of a second, the train will detect the nearest railroad track, calculate the exact curve of the rail, and apply a corrective physics force to snap the train onto the track. 
- **The Magic:** We keep the Chaos Physics engine turned *on*. This means the train will still have 10,000 tons of momentum, it can still derail if you take a corner too fast, and the air brakes will still work perfectly.

## Your Role (The Director)
As discussed, I will write the complex code. Once I push the code:
1. You will create a Blueprint from my new `ATrackSpline` class.
2. You will drag it into the 3D world and click to draw the train tracks over your mountains. 

## Open Questions for the Director
1. **Derailment Physics:** When the train takes a sharp curve too fast, do you want the C++ code to allow it to violently derail off the tracks and crash into the mountains, or do you want it "locked" to the rails permanently so it can never fall off?
