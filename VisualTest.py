import unreal

def setup_ui_blueprint():
    unreal.log("Setting up UI Blueprint...")
    # Generate BP_TrainHUDWidget
    parent_class = unreal.load_class(None, "/Script/MegaregionSim.TrainHUDWidget")
    
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    hud_path = "/Game/Blueprints/BP_TrainHUD"
    if not unreal.EditorAssetLibrary.does_asset_exist(hud_path):
        hud_bp = asset_tools.create_asset("BP_TrainHUD", "/Game/Blueprints", None, factory)
        unreal.log("Created BP_TrainHUD.")
    else:
        hud_bp = unreal.EditorAssetLibrary.load_asset(hud_path)
    
    return hud_bp

def run_visual_test(hud_bp):
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()
    
    if not world:
        unreal.log_error("No editor world found!")
        return

    # Load Blueprints
    pawn_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrainPawn")
    car_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrainCar")
    
    # 1. Spawn Locomotive
    loco_location = unreal.Vector(0, 0, 100)
    loco = unreal.EditorLevelLibrary.spawn_actor_from_object(pawn_class, loco_location)
    
    # Inject the HUD Widget Class into the Locomotive so it displays on Play
    if loco:
        loco.set_editor_property("HUDWidgetClass", hud_bp.generated_class())
        unreal.log("Assigned HUD to Locomotive.")
    
    # 2. Spawn Freight Car right behind it (Simulates Coupling Slack)
    car_location = unreal.Vector(-1100, 0, 100) # 11 meters behind
    car = unreal.EditorLevelLibrary.spawn_actor_from_object(car_class, car_location)
    
    unreal.log("Visual Test Suite ready! Press PLAY in the editor to see UI and Physics!")
    unreal.SystemLibrary.print_string(world, "UI TEST SUITE READY! PRESS PLAY!", True, True, unreal.LinearColor(0, 1, 0, 1), 10.0)

def main():
    hud_bp = setup_ui_blueprint()
    unreal.EditorAssetLibrary.save_directory("/Game/Blueprints")
    run_visual_test(hud_bp)

if __name__ == "__main__":
    main()
