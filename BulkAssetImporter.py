import unreal
import os

def run_bulk_import():
    unreal.log("--- STARTING BULK ASSET IMPORT ---")
    
    # Define paths (with fallback for exec() in Unreal)
    try:
        base_dir = os.path.dirname(__file__)
    except NameError:
        base_dir = "C:/Users/Administrator/MegaregionSim/MegaregionSim"
        
    raw_assets_dir = os.path.abspath(os.path.join(base_dir, "RawAssets", "Models"))
    unreal_dest_dir = "/Game/FinalAssets"
    
    if not os.path.exists(raw_assets_dir):
        unreal.log_error(f"Raw Assets directory not found at: {raw_assets_dir}")
        return

    # Get all FBX files
    files = [f for f in os.listdir(raw_assets_dir) if f.endswith(".fbx")]
    
    if not files:
        unreal.log_warning("No FBX files found in RawAssets/Models.")
        return
        
    tasks = []
    
    for file_name in files:
        file_path = os.path.join(raw_assets_dir, file_name)
        
        # Clean up the name for Unreal (no spaces)
        safe_name = file_name.replace(" ", "_").replace(".fbx", "")
        
        # Configure the import task
        task = unreal.AssetImportTask()
        task.filename = file_path
        task.destination_path = unreal_dest_dir
        task.destination_name = safe_name
        task.automated = True
        task.save = True
        task.replace_existing = True
        
        # Configure the FBX specific import settings
        options = unreal.FbxImportUI()
        options.import_mesh = True
        options.import_textures = True
        options.import_materials = True
        options.import_as_skeletal = False # Everything is a static mesh
        
        # CRITICAL: Auto-generate collision for physics!
        options.static_mesh_import_data.generate_lightmap_u_vs = True
        options.static_mesh_import_data.auto_generate_collision = True
        
        task.options = options
        tasks.append(task)
        
        unreal.log(f"Queued for import: {file_name} -> {safe_name}")

    # Execute all tasks
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
    
    unreal.log(f"--- BULK ASSET IMPORT COMPLETE! Imported {len(tasks)} files into {unreal_dest_dir} ---")

if __name__ == "__main__":
    run_bulk_import()
