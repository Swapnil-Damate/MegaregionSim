# Phase 1.3 Implementation Plan (V2): Interaction & Cockpit Controls

## Goal
Transform the train from a simulated physics object into a fully controllable, interactive vehicle with a first-person camera and a high-fidelity Glassmorphism HUD.

## UI Framework Decision: Native UMG vs. Web UI
*Per your question regarding performance and experience, we have finalized the UI strategy:*

**Decision:** We will use **Native UMG (Unreal Motion Graphics)** augmented with **Custom UI Materials**.
*   **Performance:** Native UMG is infinitely better for performance. It runs directly on the GPU. Using a Web UI (HTML/CSS) forces Unreal to run a hidden Google Chrome process in the background, which eats massive amounts of RAM and introduces input latency.
*   **Experience:** To achieve the "Glassmorphism" look you want without using CSS, I will program a custom Unreal Material using a "Background Blur" node. This gives us the exact same ultra-premium frosted glass aesthetic, but with zero performance drop.

## Simultaneous Execution Strategy (Subagent Delegation)
To execute this phase rapidly and cleanly, I will invoke two specialized AI subagents to work in parallel:

1. **Subagent 1 (UI Architect):** Tasked with designing the UMG Widget blueprint structure and writing the C++ UI Data bindings to perfectly sync the Speedometer and Brake Pressure gauges with the live train physics.
2. **Subagent 2 (Core Gameplay Engineer):** Tasked with modifying the `ATrainPawn` C++ class to attach a `UCameraComponent` (First-Person View) and writing the C++ bindings for Unreal Engine 5's Enhanced Input System (Throttle up/down, Brake apply/release).

## Risk Assessment & Mitigation

> [!WARNING]
> **Risk 1: C++ File Locking & Merge Conflicts**
> *Issue:* If both subagents attempt to modify `TrainPawn.cpp` simultaneously (e.g., Subagent 1 trying to add the UI bridging code while Subagent 2 adds the camera), it will result in catastrophic git merge conflicts or overwritten code.
> *Mitigation:* Strict isolation. Subagent 1 is quarantined to UI Data assets. Subagent 2 is quarantined to the core C++ logic. I (the Principal AI) will review their code and manually integrate the UI bridge into the C++ file to ensure zero code breaks.

> [!CAUTION]
> **Risk 2: Enhanced Input Context Loading**
> *Issue:* UE5 deprecated the old input system. If the `UInputMappingContext` is not explicitly loaded and assigned to the local player controller during `BeginPlay()`, the train will ignore all keyboard inputs.
> *Mitigation:* Subagent 2 will write explicit pointer checks (`if (APlayerController* PC = Cast<APlayerController>(Controller))`) and safely load the Enhanced Input Subsystem to prevent Null Pointer crashes.

## Verification Plan
1. I will orchestrate the subagents and manually merge their final C++ code to guarantee no breakage.
2. I will compile the final C++ code and push it to GitHub.
3. You will pull it on the AWS server, press Play, and instantly spawn inside the driver's seat with working W/S keys!
