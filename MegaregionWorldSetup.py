import unreal
import os

def setup_megaregion_world():
    unreal.log("--- GENERATING MEGAREGION WORLD ---")
    
    # 1. Generate 2017x2017 (standard UE5 landscape size) RAW heightmap
    resolution = 2017
    
    # Save it to the project's Saved folder
    project_dir = unreal.Paths.project_saved_dir()
    output_path = os.path.join(project_dir, "MegaregionHeightmap.raw")
    
    # Call our custom C++ function!
    success = unreal.GISDataManager.generate_procedural_heightmap_raw(
        resolution=resolution, 
        output_file_path=output_path, 
        base_scale=0.005, 
        z_scale=65535.0, 
        ghat_threshold=0.65
    )
    
    if success:
        unreal.log_warning("SUCCESS! Procedural Heightmap generated at: " + output_path)
        unreal.log_warning("To complete world setup:")
        unreal.log_warning("1. Go to Modes -> Landscape (Shift+2)")
        unreal.log_warning("2. Select 'Import from File'")
        unreal.log_warning("3. Select " + output_path)
        unreal.log_warning("4. Click Import! Your 1,000-mile PCG world is ready for Track Laying!")
    else:
        unreal.log_error("Failed to generate heightmap. Did you compile the C++ code?")

if __name__ == "__main__":
    setup_megaregion_world()
