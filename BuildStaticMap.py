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
        # Spawned far out into the procedural world to see the new hills!
        train_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(train_class, unreal.Vector(0, -10000, 300))
        unreal.log("Spawned BP_TrainPawn on the tracks.")

    # 4. Bake the Massive 40-Kilometer Procedural World!
    master_controller_class = unreal.load_class(None, '/Script/MegaregionSim.MasterSimulationController')
    if master_controller_class:
        master_controller = unreal.EditorLevelLibrary.spawn_actor_from_class(master_controller_class, unreal.Vector(0, 0, 0))
        if master_controller:
            # Trigger the C++ procedural generation to bake thousands of trees and skyscrapers instantly!
            master_controller.generate_environment()
            unreal.log_warning("SUCCESS: 40-Kilometer Procedural World with Rolling Hills Generated!")

    unreal.log("--- STATIC MAP GENERATION COMPLETE ---")
    unreal.log_warning("INSTRUCTIONS: Fly around the editor viewport! The Train is at (0, -10000, 300) surrounded by the massive world!")
    unreal.log_warning("You can save this map by going to File -> Save Current Level As -> SimulatedMap")

build_static_map()
