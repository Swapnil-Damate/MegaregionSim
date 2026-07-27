import unreal

def generate_proxy_assets():
    unreal.log("--- GENERATING PROXY ASSETS ---")
    
    # Setup Asset Tools
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    
    # Destination Folder
    dest_path = "/Game/ProxyAssets"
    
    # Engine Basic Shapes paths
    cube_path = "/Engine/BasicShapes/Cube.Cube"
    cylinder_path = "/Engine/BasicShapes/Cylinder.Cylinder"
    
    # Proxies to create (Name, SourceShape)
    proxies = [
        ("Proxy_SteamEngine", cylinder_path),
        ("Proxy_DieselEngine", cube_path),
        ("Proxy_ElectricBullet", cube_path),
        ("Proxy_PassengerCar", cube_path),
        ("Proxy_TrackStraight", cube_path),
        ("Proxy_TunnelMesh", cylinder_path),
        ("Proxy_BridgePillar", cube_path)
    ]
    
    for proxy_name, source_path in proxies:
        dest_asset = dest_path + "/" + proxy_name
        
        # Check if it already exists
        if unreal.EditorAssetLibrary.does_asset_exist(dest_asset):
            unreal.log_warning(f"Proxy already exists: {dest_asset}")
            continue
            
        # Duplicate the basic shape to act as our proxy
        duplicated_asset = unreal.EditorAssetLibrary.duplicate_asset(source_path, dest_asset)
        
        if duplicated_asset:
            unreal.log("Successfully created Proxy: " + dest_asset)
        else:
            unreal.log_error("Failed to create Proxy: " + dest_asset)
            
    unreal.log("--- PROXY GENERATION COMPLETE ---")

if __name__ == "__main__":
    generate_proxy_assets()
