# UI/HUD & Train Roster Design (V1)

This document finalizes the visual direction of the player interface and outlines the required highly detailed train models for the launch roster.

## 1. Player HUD & UI Design (Minimalist Glassmorphism)
*   **Aesthetic Core:** Modern, semi-transparent (frosted glass effect), and unobtrusive. The UI should fade into the background so the player can appreciate the extreme graphical fidelity of Unreal Engine 5.
*   **HUD Components:**
    *   **Dynamic Speedometer:** A sleek digital arc in the bottom right corner, adapting its maximum scale based on the current train (e.g., maxing at 120km/h for freight, 350km/h for HSR).
    *   **Brake & Pneumatic Panel:** Crucial for our physics engine. Displays Main Reservoir, Brake Pipe, and Brake Cylinder pressures using clean, color-coded numerical readouts.
    *   **Upcoming Track UI:** A subtle vertical timeline on the left edge of the screen showing the next 5km of track—indicating upcoming signals, speed limit changes, and stations.
    *   **Passenger/Cargo Status:** A minimalist widget in the top right showing cargo integrity (did your sloshing liquids spill?) or passenger satisfaction (are they freezing because you forgot to turn on the coach heating?).

## 2. The Launch Roster ("Everything Perfect")
Per your direction, we will not compromise by focusing on just one train type for the prototype. We will build a comprehensive, perfect roster from day one. We must build one "Masterclass" vehicle for each major gameplay category. Each train will feature a fully interactive 1:1 3D cockpit, bespoke Chaos Physics modeling, and unique MetaSounds audio profiles.

*   **The Heavy Hauler (Freight):** 
    *   *Archetype:* Modern AC Traction Diesel (e.g., WDG-4G / GE Evolution Series).
    *   *Gameplay Focus:* Managing extreme mass (10,000+ tons), wheel slip on wet mountain grades using manual sanders, and utilizing dynamic braking to prevent runaway trains on steep downgrades.
*   **The Urban Lifeline (Metro/Commuter):** 
    *   *Archetype:* High-density Electric Multiple Unit (EMU).
    *   *Gameplay Focus:* Rapid acceleration/deceleration, extremely precise station stopping (within centimeters of the platform markers), and passenger door management in the sprawling central Megacity.
*   **The Speed King (High-Speed Rail):** 
    *   *Archetype:* Next-Gen Aerodynamic Bullet Train (e.g., Vande Bharat / Shinkansen style).
    *   *Gameplay Focus:* Reading signaling systems miles in advance, managing aerodynamic drag, and maintaining strict punctuality on dedicated high-speed viaducts.
*   **The Heritage Legend (Steam):** 
    *   *Archetype:* Classic heavy steam locomotive.
    *   *Gameplay Focus:* Hardcore mechanical management—monitoring boiler pressure, water levels, coal feeding, and manually adjusting the cutoff valves. Provides an unmatched auditory and visual (volumetric smoke) atmosphere.
