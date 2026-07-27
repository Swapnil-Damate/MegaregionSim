import unreal

def generate_train_blueprint():
    unreal.log("--- GENERATING BULLETPROOF TRAIN BLUEPRINT ---")
    
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    # 1. Make Directory
    unreal.EditorAssetLibrary.make_directory('/Game/Blueprints')

    # 2. Create BP_TrainPawn Blueprint (Actor Blueprints are 100% stable in Python)
    train_parent_class = unreal.load_class(None, '/Script/MegaregionSim.TrainPawn')
    train_factory = unreal.BlueprintFactory()
    train_factory.set_editor_property("parent_class", train_parent_class)
    
    train_bp = None
    if not unreal.EditorAssetLibrary.does_asset_exist('/Game/Blueprints/BP_TrainPawn'):
        train_bp = asset_tools.create_asset("BP_TrainPawn", "/Game/Blueprints", unreal.Blueprint, train_factory)
        unreal.log_warning("Generated BP_TrainPawn successfully!")
    else:
        unreal.log_warning("BP_TrainPawn already exists.")
        train_bp = unreal.EditorAssetLibrary.load_asset('/Game/Blueprints/BP_TrainPawn')

    # Save
    unreal.EditorAssetLibrary.save_directory('/Game/Blueprints')

    # 3. Open the Train Blueprint so the user can easily adjust the camera
    if train_bp:
        unreal.AssetEditorSubsystem().open_editor_for_assets([train_bp])
        unreal.log_warning("Opened BP_TrainPawn! Please drag your LocoCamera to set your angle, then hit Compile and Save.")
        unreal.log_warning("NOTE: You do NOT need to set the HUD Widget Class anymore. The UI is 100% automated in C++ now!")

generate_train_blueprint()
