import unreal

def build_static_map():
    unreal.log("--- BUILDING STATIC SIMULATED MAP ---")
    
    # 1. Create a new map
    unreal.EditorAssetLibrary.make_directory('/Game/Maps')
    new_map_path = '/Game/Maps/SimulatedMap'
    
    # We will spawn everything into the current map. We recommend the user opens "Basic" or "Open World" first so lighting exists.
    # We don't call new_level() because we want to keep the default Unreal Engine lighting (Sun/Sky).
    
    # 2. Spawn Track
    track_class = unreal.load_class(None, '/Script/MegaregionSim.TrackGenerator')
    if track_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(track_class, unreal.Vector(0, 0, 0))
        unreal.log("Spawned Track Generator.")

    # 3. Spawn Train Blueprint on the track
    train_class = unreal.load_class(None, '/Game/Blueprints/BP_TrainPawn.BP_TrainPawn_C')
    if train_class:
        train_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(train_class, unreal.Vector(0, 0, 150))
        unreal.log("Spawned BP_TrainPawn on the tracks.")

    # 4. Spawn some Diorama Scenery so you can visualize the assets!
    # Load Skyscraper and Pine Tree meshes
    skyscraper_mesh = unreal.EditorAssetLibrary.load_asset('/Game/FinalAssets/Skyscraper')
    pine_tree_mesh = unreal.EditorAssetLibrary.load_asset('/Game/FinalAssets/Pine_Tree')
    
    # Spawn Static Mesh Actors
    sma_class = unreal.load_class(None, '/Script/Engine.StaticMeshActor')
    
    if skyscraper_mesh:
        # Spawn a small city cluster to the right of the tracks
        for i in range(3):
            city_bldg = unreal.EditorLevelLibrary.spawn_actor_from_class(sma_class, unreal.Vector(5000 + (i*2000), 4000, 0))
            if city_bldg:
                city_bldg.get_component_by_class(unreal.StaticMeshComponent).set_static_mesh(skyscraper_mesh)
                
    if pine_tree_mesh:
        # Spawn a small forest to the left of the tracks
        for i in range(10):
            tree = unreal.EditorLevelLibrary.spawn_actor_from_class(sma_class, unreal.Vector((i*1000), -3000, 0))
            if tree:
                tree.get_component_by_class(unreal.StaticMeshComponent).set_static_mesh(pine_tree_mesh)
                
    unreal.log("--- STATIC MAP GENERATION COMPLETE ---")
    unreal.log_warning("INSTRUCTIONS: Fly around the editor viewport to look at the Train, Tracks, City, and Forest!")
    unreal.log_warning("You can save this map by going to File -> Save Current Level As -> SimulatedMap")

build_static_map()
