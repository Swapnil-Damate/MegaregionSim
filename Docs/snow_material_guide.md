# Feature 8 & 50: Dynamic Snow & Dirt Accumulation Guide [STATUS: PENDING / DELTA]

As discussed in the Design Bible, we do NOT need any paid plugins to simulate snow or dirt accumulating on your trains and tracks! We have built the C++ architecture in `MegaregionWeatherSystem` to dynamically talk to your materials. 

However, since Unreal Engine materials must be visually compiled in the engine, I cannot do this via C++ text code. I need your help to configure the material nodes inside the Editor.

**Please follow these 4 simple steps in your Unreal Editor:**

## Step 1: Find Your Train Material
Since your train is a single bulky `.uasset` file (like a Diesel Locomotive mesh), you need to locate the Material attached to it:
1. In the Content Browser, **double-click** your Locomotive `.uasset` file to open the Mesh Editor.
2. In the right-hand **Details panel**, look for the **Material Slots** section.
3. You will see one or more materials assigned to the train. **Double-click** the material thumbnail (or click the magnifying glass to find it in the browser, then open it).
4. *(Note: If it opens a "Material Instance" with checkboxes, scroll down to "Parent" at the bottom of the details panel and double-click the Parent Material to open the actual node graph).*

## Step 2: Add a Weather Scalar Parameter
1. Right-click anywhere in the graph and type **ScalarParameter**. 
2. Name this exact parameter: `SnowAmount` (Case sensitive! My C++ code searches for this exact word).
3. Set the default value to `0.0`.

## Step 3: Blend the Snow
You will do this right next to the large main "Result" node (the big node with pins like Base Color, Metallic, Roughness).
1. Look at the large main Material node. Find the pin labeled **Base Color**.
2. There is likely already a texture wire plugged into **Base Color**. Hold `ALT` and click that wire to disconnect it.
3. Now, hold the `L` key and click in empty space nearby to instantly create a "Lerp" node. (Alternatively, right-click, search for **LinearInterpolate**, and select the one under the **Math** category).
4. Plug the wire from your *original* texture (that you just disconnected) into the **A** pin of the Lerp node.
5. Hold the `3` key and click in empty space to create a **Constant3Vector** node. Double-click its black box and change the color to Pure White (for Snow).
6. Plug this White color node into the **B** pin of the Lerp node.
7. Plug your new `SnowAmount` parameter node into the **Alpha** pin of the Lerp node.
8. Finally, drag a wire from the output of the Lerp node and plug it back into the **Base Color** pin of the large main Material node.

## Step 4: Apply and Save!
Hit **Apply** and **Save** in the top left corner.

---

### How it Works
When a Blizzard occurs in the game, our C++ `MegaregionWeatherSystem` will mathematically scale the `SnowAmount` parameter from `0.0` to `1.0` over 5 minutes. As this happens, your train will dynamically turn white! 

*(You can repeat this exact same process with a parameter named `DirtAmount` and a brown color node to simulate the Feature 50 maintenance mechanic!)*
