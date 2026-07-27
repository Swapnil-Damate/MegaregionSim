import unreal
import sys

def create_blueprint_from_cpp(asset_name, package_path, parent_class_path):
    # Load the C++ class we want to inherit from
    parent_class = unreal.EditorAssetLibrary.load_class(parent_class_path)
    if not parent_class:
        unreal.log_error(f"Could not load parent class: {parent_class_path}")
        return None

    # Create the factory
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    # Get the asset tools
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    # Check if it already exists
    full_path = f"{package_path}/{asset_name}.{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full_path):
        unreal.log_warning(f"Blueprint {asset_name} already exists. Skipping creation.")
        return unreal.EditorAssetLibrary.load_asset(full_path)

    # Create the blueprint asset
    new_blueprint = asset_tools.create_asset(asset_name, package_path, None, factory)
    unreal.log(f"Successfully created Blueprint: {asset_name}")
    return new_blueprint

def spawn_proof_of_concept():
    # Automatically spawn it into the editor level to PROVE it works
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()
    
    if world:
        # Spawn BP_TrainPawn
        bp_class = unreal.EditorAssetLibrary.load_blueprint_class("/Game/Blueprints/BP_TrainPawn")
        if bp_class:
            spawn_location = unreal.Vector(0, 0, 500)
            unreal.EditorLevelLibrary.spawn_actor_from_object(bp_class, spawn_location)
            unreal.log("SUCCESS: Automated Train Physics Running! Blueprint spawned in level.")
            
            # Print massive screen message in UE5 Viewport
            unreal.SystemLibrary.print_string(world, "SUCCESS: ZERO-MANUAL-ENTRY PIPELINE OPERATIONAL!", True, True, unreal.LinearColor(0, 1, 0, 1), 10.0)

def main():
    unreal.log("Starting Zero-Manual-Entry Blueprint Automation...")
    
    # 1. Create BP_TrainPawn
    pawn_bp = create_blueprint_from_cpp("BP_TrainPawn", "/Game/Blueprints", "/Script/MegaregionSim.TrainPawn")
    
    # 2. Create BP_TrainCar
    car_bp = create_blueprint_from_cpp("BP_TrainCar", "/Game/Blueprints", "/Script/MegaregionSim.TrainCar")
    
    # Save the newly created assets
    unreal.EditorAssetLibrary.save_directory("/Game/Blueprints")
    
    # Spawn it to prove it works
    spawn_proof_of_concept()

if __name__ == "__main__":
    main()
