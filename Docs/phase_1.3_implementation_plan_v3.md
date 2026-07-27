# Phase 1.3 Implementation Plan (V3): Automated Web UI

## Goal
Scrap the manual Native UMG workflow and pivot to a fully automated **HTML/CSS/JS Web UI architecture**. This allows me (the AI) to generate and push stunning Glassmorphism interfaces automatically, requiring zero manual node-dragging from you in the future.

## Architectural Shift
Instead of dragging 4 Text nodes, we are going to use Unreal Engine's built-in **Web Browser Widget**.
1. I will write a massive, beautifully animated HTML/CSS file containing the dashboard, speedometer, and pressure gauges, styled with CSS backdrop-filters for true Glassmorphism.
2. I will write C++ code that silently loads this HTML file and injects it directly into the Unreal Engine Web Browser.
3. Every 1/60th of a second, the C++ physics engine will execute JavaScript commands inside the browser to dynamically update the numbers on the screen.

## Proposed Code Changes

### 1. Engine Configuration
- **[MODIFY]** `MegaregionSim.uproject` - Enable the `"WebBrowserWidget"` plugin.
- **[MODIFY]** `MegaregionSim.Build.cs` - Add the `"WebBrowserWidget"` and `"WebBrowser"` modules to the compiler.

### 2. The Web UI Asset
- **[NEW]** `Source/MegaregionSim/UI/HUD.html` - The raw HTML/CSS/JS file. I will build a sleek, dark-mode Glassmorphism dashboard with glowing fonts and dynamic Javascript functions.

### 3. C++ UI Bridge
- **[MODIFY]** `TrainHUDWidget.h` & `TrainHUDWidget.cpp`
  - Remove all the manual `UTextBlock` bindings.
  - Add a single `UWebBrowser* UIBrowser` binding.
  - Write file-loading logic in `NativeConstruct()` to read `HUD.html` from the hard drive and inject it into the browser.
  - Rewrite `UpdateHUDMetrics()` to trigger `ExecuteJavascript()` to push live physics data to the HTML.

## User Action Required (One-Time Setup)
If you approve this plan, I will push all the code. Because I enabled a new plugin, you will have to:
1. Pull the code.
2. Refresh the Visual Studio project.
3. Open Unreal Engine.
4. Open the `WBP_TrainHUD` blueprint, delete the 4 Text nodes, drag in exactly **one** "Web Browser" node, and name it `UIBrowser`. 

*You will never have to touch a UI node again after this.*

## Verification Plan
1. I will orchestrate this architectural shift and push the code.
2. You will perform the one-time manual setup.
3. When you press Play, you will see a breathtaking, fully functional web-based dashboard overlaid on the 3D world!
