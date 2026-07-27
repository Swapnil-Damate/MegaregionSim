# Phase 1.3 Implementation Plan (V1): Interaction & Cockpit Controls

## Goal
Transform the train from a simulated physics object into a fully controllable, interactive vehicle with a first-person camera and a high-fidelity Glassmorphism HUD.

## Simultaneous Execution Strategy (Subagent Delegation)
To execute this phase rapidly and cleanly, I will invoke two specialized AI subagents to work in parallel in the background:

1. **Subagent 1 (Front-End Architect):** Tasked purely with designing the HTML/CSS/JS frontend for the Glassmorphism UI (Speedometer, Brake pressure gauges). They will build this in a local web folder so we can test it in a standard browser before porting it to Unreal.
2. **Subagent 2 (Core Gameplay Engineer):** Tasked with modifying the `ATrainPawn` C++ class to attach a `UCameraComponent` (First-Person View) and writing the C++ bindings for Unreal Engine 5's Enhanced Input System (Throttle up/down, Brake apply/release).

## Risk Assessment & Mitigation

> [!WARNING]
> **Risk 1: C++ File Locking & Merge Conflicts**
> *Issue:* If both subagents attempt to modify `TrainPawn.cpp` simultaneously (e.g., Subagent 1 trying to add the UI bridging code while Subagent 2 adds the camera), it will result in catastrophic git merge conflicts or overwritten code.
> *Mitigation:* Strict isolation. Subagent 1 is strictly quarantined to writing HTML/CSS files. Subagent 2 is quarantined to the C++ logic. I (the Principal AI) will review their code and manually integrate the UI bridge into the C++ file to ensure zero code breaks.

> [!CAUTION]
> **Risk 2: Enhanced Input Context Loading**
> *Issue:* UE5 deprecated the old input system. If the `UInputMappingContext` is not explicitly loaded and assigned to the local player controller during `BeginPlay()`, the train will ignore all keyboard inputs.
> *Mitigation:* Subagent 2 will write explicit pointer checks (`if (APlayerController* PC = Cast<APlayerController>(Controller))`) and safely load the Enhanced Input Subsystem to prevent Null Pointer crashes.

## Open Questions for the Human Director
1. **UI Framework Decision:** Are you comfortable using a Web UI (HTML/CSS via an Unreal Web Browser plugin) for the HUD? This allows for the absolute best Glassmorphism aesthetics, but requires you to enable the "Web Browser" plugin in the Unreal Engine editor. Or do you prefer we stick to native UMG (Unreal Motion Graphics)?

## Verification Plan
1. I will orchestrate the subagents and review their code.
2. I will compile the final C++ code and push it to GitHub.
3. You will pull it on the AWS server, enable any necessary plugins, and press Play to see the camera inside the train and press W/S to move it.
