# Master Game Design Document: Core Scope (V1 Final)

This document outlines the in-scope features for our highly realistic 3D offline simulation game, focusing on world dynamics, realistic physics, complex AI behaviors, and transport systems. 

*Note: Multiplayer features have been explicitly excluded. The game is designed as a fully offline, single-player experience.*

## 1. World & Environment
- **Procedural/Seamless Terrain & Biomes:** Highly detailed nature and landscapes. Dense forests with individual plants/underbrush, varied rock formations, and distinct geological zones. 
- **Extreme Terrains & Ghat Sections (New):** Tracks spread across diverse topographies, emphasizing extreme terrains like Ghat sections and mountain passes. These areas demand mastery of gravity, traction, and braking on steep gradients, featuring realistic track superelevation (banking) on curves.
- **Dynamic Urban & Rural Landscapes:** Vast cities with diverse architectural styles, sprawling suburbs, industrial zones, and remote villages. Buildings are detailed down to the micro-level.
- **Advanced Environment & Astronomy:** Full day/night cycle with distinct dawn, dusk, and evening phases. Astronomical events (solar/lunar eclipses, auroras, meteor showers). Realistic weather systems (fog, rain, clear skies, desert heat, sunny, snow) directly affecting traction, visibility, and thermal dynamics.
- **Era-Specific Infrastructure & Effects (New):** Deep simulation of infrastructure required for various train types:
  - **Steam/Coal Era:** Functioning coaling towers, water cranes, and dynamic volumetric smoke/soot that realistically stains tunnels and nearby buildings over time.
  - **Electric Era:** Fully simulated catenary grids (overhead wires) with pantograph tension physics, electrical sparks during rain, and power substation management.
- **Massive Infrastructure Networks:** Extremely complex, large-scale rail networks stretching hundreds of miles. Intricate switching yards, sprawling multi-level interchange stations, realistically modeled passenger terminals, functional bridges, and detailed railway crossings.
- **Macro-to-Micro Ecosystems & Disasters:** Ecological simulation where weather impacts the terrain. Flash floods washing out tracks, rain causing soil erosion, droughts leading to wildfires, and winter avalanches blocking mountain passes.
- **Long-Term World Progression:** The world evolves over time. Cities grow, new industries develop, and transportation demands dynamically shift, requiring network expansion.
- **Geological Aging & Upgrades:** Environmental wear-and-tear on structures (rust, weathering). This ties into an upgrade system where you can modernize infrastructure (e.g., upgrading wooden ties to concrete, old semaphores to digital signals).

## 2. Vehicle & Train Operations
- **Expansive Transport Roster:** A massive timeline of transport. Steam (coal/wood), Diesel, Electric, and futuristic Hydrogen-cell locomotives. Includes Metro networks, Monorails, urban Trams, and Superfast/High-Speed Rail (HSR).
- **Diverse Rolling Stock:** Passenger cars (including double-deckers and luxury sleepers), varied cargo (refrigerated, liquid tankers, hazardous materials, and bulk solids like coal, wood, and gravel), and specialized mail/logistics trains.
- **Hardcore Simulation Physics (Expanded):** 
  - Realistic acceleration, mass/inertia, braking distance, and track-following physics. 
  - **Pneumatic Brake Simulation:** Simulating the realistic delay in brake pipe pressure across a massive, multi-mile freight train.
  - **Traction & Adhesion:** Managing wheel slip on wet/steep tracks using manual sanders.
  - **Coupling Dynamics:** High-stress coupling physics where poor throttle management can break train knuckles (couplers) on heavy grades.
- **Advanced Cockpits (Software-Based):** 1:1 scale, fully interactive virtual cockpits. Every lever, dial, and button is clickable and functional. Features complex cold-start procedures directly within the software UI.
- **Component-Level Physical Simulation:** Soft-body & rigid-body fusion for realistic deformation during crashes or derailments based on material science.
- **Dynamic Cargo Physics (Expanded):** 
  - Fluid dynamics for liquid cargo sloshing (shifting center of gravity).
  - Bulk solid shifting (coal or gravel sliding in hoppers during sharp turns).
  - Improperly secured wood or pipes rolling off flatbeds during sudden stops.

## 3. AI Systems (Entities & Traffic)
- **Massive Rail Traffic Model (New):** A highly complex, background-simulated AI train network. Hundreds of AI trains (local commuters, heavy freight, and superfast high-speed trains blowing past at 300+ km/h) populate the world, governed by AI dispatchers that dynamically route traffic to avoid collisions.
- **Deep Passenger Simulation:** Individual passenger AI simulating satisfaction levels based on ride smoothness, temperature in the coach, punctuality, and ticket pricing.
- **Dynamic Pedestrians & Wildlife:** Autonomous animals and human NPCs with pathfinding reacting dynamically to trains, weather, and terrain.
- **Signal & Rail AI:** Automated train dispatching, complex switch point management in massive yards, and realistic signal safety systems (e.g., PTC, ETCS).
- **"Living" NPC Routines:** Agent-Based Modeling where NPCs have routines, jobs, and homes, driving realistic economic and traffic flows.
- **Consequence-Driven Emergency AI:** Dynamic AI rerouting during accidents, with emergency services physically navigating to and securing accident sites.

## 4. Maintenance & Narrative (New Focus)
- **Story-Driven Career & Legacy:** Maintenance isn't just a chore; it's part of a broader narrative. Inherit a rundown depot and restore vintage, rusted locomotives to their former glory for a transport museum. Alternatively, maintain your modern fleet to uphold your company's safety rating, unlocking lucrative government logistics contracts.
- **Physical Maintenance Loops:** Perform train washing, interior cleaning, repaint custom liveries, and replace worn-out mechanical components in a fully rendered depot environment.

## 5. Graphics & Atmosphere
- **Rendering & Lighting:** High-fidelity lighting, realistic shadow cascades, and distance-based fog/atmospheric scattering.
- **Immersive Audio:** Realistic audio loops for engine pitch, rail clickity-clack, ambient nature, and crossing signals.
- **Ray-Traced Audio (Advanced):** Acoustic ray tracing for realistic sound bouncing, echoing, and muffling based on 3D geometry.
- **Volumetric Weather (Advanced):** Voxel-based volumetric clouds casting shadows and physically interacting with terrain.

## 6. Tech Stack, Architecture & Offline Cross-Platform Support
- **Engine/Framework:** Unreal Engine 5 (C++) remains the primary recommendation for rendering and physics capabilities.
- **Fully Offline Architecture:** No server dependencies; all AI, physics, and world generation are calculated locally.
- **Cross-Platform Viability (Android & MacOS):**
  - **MacOS:** Fully supported. Modern Apple Silicon (M1/M2/M3) handles UE5 incredibly well, allowing for high-end graphical fidelity comparable to PC.
  - **Android/Mobile (Adaptive Profiling):** The game will feature an intelligent, automatic hardware detection system. On mobile devices, it will dynamically adjust graphics and disable unsupported/extreme features (like Nanite/Lumen, extreme deformation physics, or HD textures) to ensure smooth offline performance and prevent device overheating or battery damage.
- **Architecture:** Modular, decoupled components (Physics, Audio, AI Controller, Render Pipeline) utilizing an Entity Component System (ECS) for massive AI/physics processing.
- **Workflow:** Procedural generation pipelines (e.g., Houdini Engine) for scalable world-building.

---

> [!NOTE]
> This is a living document. Please review and let me know what new features, systems, or ideas you would like to add next!
