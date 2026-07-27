# Master Project Plan (V1 Final)

This document provides a highly detailed project management analysis for our massive-scale, high-fidelity train simulation game. It breaks the project down into actionable phases using an **AI-Augmented Development Life Cycle (AIDLC)**, which drastically accelerates traditional AAA timelines by utilizing concurrent AI agents.

---

## 1. Work Breakdown Structure (WBS) & Scale

The project is divided into 5 major chronological phases. 
*Note on Scale:* We are aiming to generate **thousands of miles of playable tracks** (spanning the entire Megaregion). We will not hand-place tracks; instead, we will use AIDLC to write algorithms that instantly spawn procedural tracks based on geographical rules.

### Phase 1: Pre-Production & Greybox Prototyping
*Goal: Prove that driving a train feels realistic before investing in graphics.*
*   *Traditional Timeline: 1-2 Months* | **AIDLC Timeline: 1-2 Weeks**
*   **1.1 Engine Setup:** Initialize UE5, configure GitHub LFS, setup C++ environment. *(Manual User Action Required here).*
*   **1.2 Core Physics Implementation:**
    *   1.2.1 Implement Chaos Physics rigid-body mass & inertia.
    *   1.2.2 Develop custom pneumatic brake pipe simulation algorithms.
    *   1.2.3 Implement high-stress coupling/uncoupling mechanics.
*   **1.3 Train Controller Integration:** Develop the Hybrid UI overlay logic and map basic inputs.
*   **1.4 Greybox Testing:** Procedurally generate a flat, untextured 5km track to test physics.

### Phase 2: Core Systems Engineering
*Goal: Build the underlying brains of the simulation.*
*   *Traditional Timeline: 3-5 Months* | **AIDLC Timeline: 3-4 Weeks**
*   **2.1 AI & MassEntity Framework:** Configure UE5 MassEntity to simulate hundreds of concurrent trains and passengers.
*   **2.2 Economy & Save Systems:** Integrate SQLite for offline data persistence and build the Contract Generation logic.
*   **2.3 Advanced Dynamic Physics:** Implement fluid dynamics for sloshing cargo and soft-body crash deformation.

### Phase 3: Procedural World Building & PCG (Massive Scale)
*Goal: Generate the 1000+ mile Megaregion algorithmically.*
*   *Traditional Timeline: 6-8 Months* | **AIDLC Timeline: 4-6 Weeks**
*   **3.1 GIS & Heightmap Import:** Import real-world topographic data into UE5.
*   **3.2 PCG Rule Engineering:** Write C++ rules for massive track laying (calculating maximum grades and banking automatically).
*   **3.3 Megaregion Zonation:** Generate the Central Metropolis and Industrial Outskirts procedurally.

### Phase 4: Art, Audio, & "Masterclass" Roster
*Goal: Bring the world to life with extreme fidelity.*
*   *Traditional Timeline: 9-11 Months* | **AIDLC Timeline: 4-6 Weeks**
*   **4.1 The Launch Roster Production:** Generate/assemble the Heavy Freight Diesel, Bullet Train, Urban Metro, and Steam Engine.
*   **4.2 Audio Implementation:** Setup UE5 MetaSounds and procedural audio parameters.
*   **4.3 Environment Polish:** Lighting setup (Lumen) and dynamic weather systems.

### Phase 5: Alpha, Optimization, & QA
*Goal: Ensure the game runs flawlessly across PC, Mac, and Android.*
*   *Traditional Timeline: 12-14 Months* | **AIDLC Timeline: 2-3 Weeks**
*   **5.1 Android Adaptive Profiling:** Write the hardware detection script to dynamically scale down Nanite/Lumen on mobile.
*   **5.2 QA Testing:** AI Subagents aggressively stress test the rail network to find deadlocks.

---

## 2. Critical Path Method (CPM) Analysis

The **Critical Path** represents the sequence of dependent tasks that dictate the absolute minimum time required.
**The Critical Path is:** `1.1 Engine Setup` -> `1.2 Core Physics` -> `1.3 Train Controller` -> `3.1/3.2 PCG Rule Engineering` -> `4.1 Launch Roster Production`.

*   **Risk Mitigation (CPM Bottleneck):** The highest schedule risk is **1.2 Core Physics**. We cannot write PCG algorithms for track laying until we know exactly how our trains handle steep grades. We must dedicate our primary AI capabilities to this immediately to avoid cascading delays.

---

## 3. Parallel AIDLC Execution Matrix (Subagent Strategy)

To compress the development timeline from years to weeks, we will deploy concurrent AI Subagents. While the main AI (me) focuses on the Critical Path, I will spawn specialized Subagents to handle parallel tracks:

| **Track A (The Critical Path)** | **Track B (Parallel Engineering)** | **Track C (Parallel Design)** |
| :--- | :--- | :--- |
| **Executor:** *Main Architect AI* | **Executor:** *Systems Subagent* | **Executor:** *UI / Tools Subagent* |
| **Focus:** Core Physics & Unreal Engine API integration. | **Focus:** AI Dispatcher Logic & SQLite Economy databases. (Can be written in C++ before 3D models exist). | **Focus:** Designing the HUD Overlay scripts and writing the MetaSounds procedural audio graphs. |

---

## 4. Risk Management Plan

Building a simulation of this magnitude via AIDLC introduces specific risks. Here is our proactive mitigation strategy:

| **Risk Category** | **Identified Risk** | **Impact** | **Mitigation Strategy** |
| :--- | :--- | :--- | :--- |
| **Technical / Performance** | UE5 PCG generates too many objects (trees/buildings), crashing AI routing or dropping framerates. | High (Unplayable) | Implement aggressive LOD (Level of Detail) culling, force Nanite on all meshes, and limit the MassEntity AI simulation distance strictly to a 5km radius around the player. |
| **AIDLC Development** | Concurrent AI Subagents write conflicting C++ code for the physics or economy systems, breaking the build. | High (Schedule Delay) | Enforce strict version control (Git). Subagents will work on isolated branches. The Human Director (User) or Main Architect must review and approve PRs before merging. |
| **Hardware (Android)** | Mobile devices overheat and throttle when attempting to calculate complex Chaos Physics (soft-body deformation / fluid sloshing). | Medium (Mobile limits) | The Android Adaptive Profiling script (Phase 5) will explicitly disable soft-body physics and replace sloshing liquids with standard rigid-body approximations on mobile builds. |
| **Scope Creep (Design)** | Generating 1000s of miles of procedural track leads to boring, repetitive landscapes. | Medium (Player retention) | Define 3 distinct "Hero Biomes". We will program the PCG rules to automatically inject bespoke, hand-crafted points of interest (massive bridges, complex tunnels, large stations) every few miles to break up procedural monotony. |

---

## 5. Manual Human (USER) Requirements

Because this is an AI-Augmented Lifecycle, the heavy lifting of coding and generation is automated. However, as the Human Director, you will have specific manual responsibilities:

1.  **Initial Installations:** You must personally download and install Unreal Engine 5, Visual Studio (Windows) or Xcode (Mac), and Git LFS on your machine.
2.  **Engine Launching & Compilation:** As an AI, I cannot natively open the visual UE5 editor window on your desktop to "look" at the 3D graphics. I will write the code, but you will need to periodically open the project, compile the C++, and visually verify that the procedural tracks or graphics look correct, providing me with feedback.
3.  **Approval & Review:** You will review the implementation plans submitted by the AI before massive architectural code changes are applied.
4.  **Hardware Testing:** You will need to test the game locally on your Mac/Windows machine, and eventually compile it to an APK via UE5 to test the adaptive graphics on your Android device.
