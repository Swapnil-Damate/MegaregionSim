import unreal

def generate_blueprints():
    unreal.log("--- GENERATING MEGAREGION BLUEPRINTS ---")
    
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    # 1. Make Directory
    unreal.EditorAssetLibrary.make_directory('/Game/Blueprints')
    
    # 2. Create Train HUD Widget Blueprint
    # Note: Python cannot directly create a 'WidgetBlueprint' via normal factories easily, 
    # so we create a standard Blueprint derived from TrainHUDWidget (if it exists) or UserWidget.
    hud_class = unreal.load_class(None, '/Script/MegaregionSim.TrainHUDWidget')
    if not hud_class:
        hud_class = unreal.load_class(None, '/Script/UMG.UserWidget')
        
    hud_factory = unreal.BlueprintFactory()
    hud_factory.set_editor_property("parent_class", hud_class)
    
    hud_bp = None
    if not unreal.EditorAssetLibrary.does_asset_exist('/Game/Blueprints/WBP_TrainHUD'):
        hud_bp = asset_tools.create_asset("WBP_TrainHUD", "/Game/Blueprints", unreal.Blueprint, hud_factory)
        unreal.log_warning("Generated WBP_TrainHUD!")
    else:
        unreal.log_warning("WBP_TrainHUD already exists.")
        hud_bp = unreal.EditorAssetLibrary.load_asset('/Game/Blueprints/WBP_TrainHUD')

    # 3. Create BP_TrainPawn Blueprint
    train_parent_class = unreal.load_class(None, '/Script/MegaregionSim.TrainPawn')
    train_factory = unreal.BlueprintFactory()
    train_factory.set_editor_property("parent_class", train_parent_class)
    
    train_bp = None
    if not unreal.EditorAssetLibrary.does_asset_exist('/Game/Blueprints/BP_TrainPawn'):
        train_bp = asset_tools.create_asset("BP_TrainPawn", "/Game/Blueprints", unreal.Blueprint, train_factory)
        unreal.log_warning("Generated BP_TrainPawn!")
    else:
        unreal.log_warning("BP_TrainPawn already exists.")
        train_bp = unreal.EditorAssetLibrary.load_asset('/Game/Blueprints/BP_TrainPawn')

    # Save all generated assets
    unreal.EditorAssetLibrary.save_directory('/Game/Blueprints')

    # Open the Train Blueprint so the user can easily adjust the camera and UI class!
    if train_bp:
        unreal.AssetEditorSubsystem().open_editor_for_assets([train_bp])
        unreal.log_warning("Opened BP_TrainPawn! Please set the Camera Angle and HUDWidgetClass in the Details Panel manually, then hit Compile and Save.")

generate_blueprints()
