# Technical Stack & Dependency Matrix (V1 Final)

This document outlines the finalized, cost-effective (free/open-source focused) technology stack and dependency matrix required for the simulation game.

## 1. Core Engine & Platforms
*   **Game Engine Decision: Unreal Engine 5 (UE5) vs. Godot Engine**
    *   *Our Choice: Unreal Engine 5 (UE5)*
    *   **Unreal Engine 5:** Free to use (royalty only applies after $1M USD gross revenue). We are choosing UE5 because our V1 Scope demands "highly realistic" graphics, extreme terrain scale, and micro-detail rendering. UE5 natively supports this via Nanite (virtualized geometry) and Lumen (global illumination).
    *   **Godot Engine (Consideration):** Godot is 100% free and open-source (no royalties ever) and fantastic for small to mid-tier 3D games. However, in our context (massive open worlds, extreme realistic physics, and next-gen lighting), Godot currently lacks the out-of-the-box AAA rendering technologies (like Nanite) and massive open-world streaming optimizations that UE5 provides. Using Godot would require us to lower our graphical threshold significantly.
    *   *Platform Support:* Fully supports building for **Windows**, **macOS**, and **Android** from a single project.
*   **Programming Languages:**
    *   **C++:** For core physics and performance-critical systems.
    *   **Blueprints (Visual Scripting):** For rapid game logic and tying systems together.

## 2. Simulation Frameworks (Built-in UE5)
*   **Physics Engine: Chaos Physics**
    *   *Why:* Epic's native physics solver. We will rely on it heavily for the hardcore train dynamics, soft-body deformation during crashes, and fluid sloshing for cargo.
*   **AI & Entity Management: MassEntity (ECS - Entity Component System)**
    *   *Why:* Crucial for our game. To simulate hundreds of AI trains, dynamic dispatchers, and thousands of individual passengers without lagging the game, we must use an ECS architecture rather than traditional object-oriented actors. MassEntity is UE5's highly optimized, data-oriented AI framework designed specifically for this scale.
*   **UI & Telemetry: Unreal Motion Graphics (UMG) & Slate**
    *   *Why:* For building the complex, interactive 1:1 software-based virtual train cockpits and deep external telemetry dashboards.

## 3. World Building & Procedural Generation (Script-Driven)
*   *Note: We have dropped external paid tools (like Houdini) to keep this cost-effective and script-driven.*
*   **Terrain & Tracks: UE5 PCG (Procedural Content Generation) Framework**
    *   *Setup:* Built directly into Unreal Engine 5. No external downloads required.
    *   *Quality Assurance:* **Yes, the high-quality threshold will be fully maintained.** UE5 PCG is a state-of-the-art AAA tool used by Epic Games to build ultra-realistic demos (like *The Matrix Awakens*). It doesn't sacrifice quality; it simply means we will rely on writing complex algorithms (in C++ and PCG Graphs) rather than using a third-party UI tool like Houdini.
    *   *How it works:* We will write scripts to automatically generate mountains, valleys, and lay down tracks mathematically.
*   **Foliage:** Quixel Megascans (Trees & Plants) distributed procedurally via UE5 PCG. (Free for UE5 users, extremely high photographic quality).

## 4. 3D Modeling & Assets (Script-Assisted)
*   *Note: Since you prefer a programmatic approach without manual modeling, we will rely on code and free libraries.*
*   **Procedural Meshes:** We will use C++ to programmatically generate basic infrastructure geometry directly in the engine.
*   **Asset Libraries: Quixel Megascans & Epic Marketplace**
    *   *Usage:* For complex textures (rust, metal) and realistic environmental objects. I will help write scripts to dynamically assemble these pre-made, free parts into complex structures (like train stations or modular trains).

## 5. Audio & Data Persistence
*   **Audio Engine: UE5 MetaSounds**
    *   *Why:* We will drop paid external tools (Wwise/FMOD) and use Unreal's built-in MetaSounds. It is highly programmable via scripts/Blueprints and capable of dynamic procedural audio (like engine RPM variations) entirely for free.
*   **Data Persistence (Save System): SQLite + USaveGame**
    *   *Why:* Since the game features long-term world progression and logistics economics, we need a robust database to save state across play sessions securely and efficiently offline.

## 6. Version Control & Project Management
*   **Repository: GitHub (Git + Git LFS)**
    *   *Why:* Free and accessible. We will configure Git Large File Storage (LFS) to handle the 3D assets and textures without needing expensive enterprise software like Perforce.
*   **Project Management: GitHub Projects / Issues**
    *   *Why:* Built directly into GitHub. Free, automated, and requires no external APIs or Jira setups.

## 7. Dependency Matrix (Required Downloads for You)
To get started with this stack, you will only need to install a few free foundational tools.

| Dependency | Purpose | Cost |
| :--- | :--- | :--- |
| **Epic Games Launcher & Unreal Engine 5** | The core development environment. | Free |
| **Xcode (Mac) / Visual Studio (Windows)** | IDE for compiling the C++ code. | Free |
| **Git & Git LFS** | To pull/push code from the GitHub repository. | Free |
| **GitHub Desktop (Optional)** | UI tool for managing Git commits easily. | Free |
