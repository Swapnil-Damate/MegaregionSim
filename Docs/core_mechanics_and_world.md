# Core Mechanics & World Design (V1.1)

This document outlines the finalized design decisions for the game's core loop, economic progression, and world map structure.

## 1. UI & Train Controls (The "Hybrid" Approach)
*   **Design Philosophy:** Balancing hardcore simulation with accessibility across PC, Mac, and Mobile platforms.
*   **Implementation:**
    *   **3D Interactive Cockpit:** The cockpit remains fully modeled. Levers and buttons physically move when interacted with, maintaining the immersive simulation feel.
    *   **2D HUD Overlay:** A sleek, modern 2D interface will overlay the screen. This provides critical telemetry (speed, brake pipe pressure, signal distance, and passenger satisfaction) at a glance. 
    *   **Why:** This prevents the frustration of trying to click tiny 3D buttons on a mobile screen or trackpad, while still allowing hardcore players to use the physical cockpit if they choose.

## 2. Economy & Logistics (Driver-Centric Progression)
*   **Design Philosophy:** The player is fundamentally a **Locomotive Driver**. The company progression exists to give meaning to your driving, not to turn the game into a spreadsheet manager.
*   **Implementation:** 
    *   You take on generated driving contracts (e.g., "Drive 500 tons of coal through the mountain pass by midnight" or "Drive the morning Metro run and maintain a 90% passenger satisfaction rating").
    *   **You must physically drive the train to complete the contract.**
    *   Completing contracts awards you capital. You then use this capital to:
        *   Purchase new trains to drive (unlocking eras from Steam to Electric).
        *   Upgrade the infrastructure of the routes you drive (e.g., upgrading wooden tracks to high-speed rail).
    *   *Failure state:* Failing a contract simply yields no money and a temporary drop in company reputation (slower progression), rather than ending the game.

## 3. World Map Design (The Authentic "Megaregion")
*   **Design Philosophy:** Instead of a strict 1:1 real-world replica, we will design a massive, fictional **Megaregion**. This gives us the creative freedom to include every type of train network (Metro, Monorail, Heavy Freight) in a way that feels organic and highly realistic, without being constrained by real-world geography.
*   **Implementation:**
    *   **The Urban Core (Metropolis):** A sprawling central megacity. This is where the dense, complex networks live—underground subways, elevated monorails, and street-level trams weaving through traffic.
    *   **The Industrial Outskirts:** Massive switching yards and logistics hubs surrounding the city, catering to heavy diesel and electric freight trains.
    *   **The Extreme Frontiers (Mountains/Ghats):** Tracks leaving the city eventually transition into treacherous, highly realistic mountain passes inspired by the Swiss Alps and Indian Ghats. These routes require steam or heavy diesel engines to conquer steep gradients, deep tunnels, and extreme weather.
    *   **Why this works:** It allows a player to seamlessly drive a high-speed passenger train out of a modern city, and later switch to a heavy freight train hauling cargo up a dangerous, winding mountain pass—all within the exact same cohesive map.
