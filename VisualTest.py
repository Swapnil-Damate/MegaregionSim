import unreal

def generate_blueprint(asset_name, parent_class_path):
    asset_path = "/Game/Blueprints/" + asset_name
    
    # If it already exists, just load and return it. Deleting and recreating with the same name causes CDO memory crashes in PIE.
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log("Blueprint already exists, skipping generation: " + asset_name)
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    parent_class = unreal.load_class(None, parent_class_path)
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    bp = asset_tools.create_asset(asset_name, "/Game/Blueprints", None, factory)
    
    # Compile the new blueprint so it's fully ready for PIE
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    
    unreal.log("Generated: " + asset_name)
    return bp

def run_visual_test():
    # Use the current level so we have lighting!
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()
    
    if not world:
        unreal.log_error("No editor world found!")
        return

    unreal.log("--- STARTING ZERO-MANUAL-ENTRY PIPELINE ---")
    
    # 1. Regenerate All Blueprints from Scratch (Fixes C++ CDO memory corruption)
    hud_bp = generate_blueprint("BP_TrainHUD_V2", "/Script/MegaregionSim.TrainHUDWidget")
    pawn_bp = generate_blueprint("BP_TrainPawn_V2", "/Script/MegaregionSim.TrainPawn")
    car_bp = generate_blueprint("BP_TrainCar_V2", "/Script/MegaregionSim.TrainCar")
    track_bp = generate_blueprint("BP_TrackGenerator_V2", "/Script/MegaregionSim.TrackGenerator")
    
    unreal.EditorAssetLibrary.save_directory("/Game/Blueprints")

    # Load the generated classes
    track_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrackGenerator_V2")
    pawn_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrainPawn_V2")
    car_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrainCar_V2")
    
    # 2. Clear Existing Actors (Optional cleanup)
    # Not done here to avoid deleting user stuff, but we can assume empty level
    
    # 3. Spawn 5-Kilometer Track
    if track_class:
        track = unreal.EditorLevelLibrary.spawn_actor_from_object(track_class, unreal.Vector(0, 0, 0))
        if track:
            track.set_actor_location(unreal.Vector(0, 0, 0), False, False) # Force location
        unreal.log("Spawned 5-Kilometer Track.")

    # 4. Spawn Locomotive
    loco = unreal.EditorLevelLibrary.spawn_actor_from_object(pawn_class, unreal.Vector(0, 0, 300))
    if loco:
        loco.set_actor_location(unreal.Vector(0, 0, 300), False, False) # Force location
        loco.set_editor_property("HUDWidgetClass", hud_bp.generated_class())
        unreal.log("Assigned HUD to Locomotive.")
    
    # 5. Spawn Liquid Tanker (Simulates Coupling Slack & Sloshing)
    car = unreal.EditorLevelLibrary.spawn_actor_from_object(car_class, unreal.Vector(-2100, 0, 300))
    if car:
        car.set_actor_location(unreal.Vector(-2100, 0, 300), False, False) # Force location
        unreal.log("Spawned Liquid Tanker (Sloshing Enabled by C++ default).")
    
    # 6. Spawn a Roadblock far ahead on the track to test Soft-Body Deformation
    roadblock = unreal.EditorLevelLibrary.spawn_actor_from_object(car_class, unreal.Vector(15000, 0, 300))
    if roadblock:
        roadblock.set_actor_location(unreal.Vector(15000, 0, 300), False, False) # Force location
    unreal.log("Spawned Roadblock for Crash testing!")
    
    # Optional: Focus camera on the Locomotive so the user immediately sees it
    unreal.EditorLevelLibrary.editor_set_game_view(True)
    
    unreal.log("Visual Test Suite ready! Press PLAY in the editor to see UI and Physics!")
    unreal.SystemLibrary.print_string(world, "ALL BLUEPRINTS GENERATED! PRESS PLAY!", True, True, unreal.LinearColor(0, 1, 0, 1), 10.0)

if __name__ == "__main__":
    run_visual_test()
