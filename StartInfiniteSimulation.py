import unreal

def start_infinite_sim():
    unreal.log("--- STARTING INFINITE MEGAREGION SIMULATION ---")
    
    # 1. Clear old actors
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor.get_class().get_name() in ['PlayerStart', 'MasterSimulationController', 'TrackGenerator', 'InfiniteWorldGenerator']:
            unreal.EditorLevelLibrary.destroy_actor(actor)

    # 1.5 Spawn a perfect PlayerStart
    player_start_class = unreal.load_class(None, '/Script/Engine.PlayerStart')
    if player_start_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(player_start_class, unreal.Vector(0, 0, 150))
        unreal.log_warning("SPAWNED: Perfect PlayerStart at 0,0,150")

    # 2. Spawn the Infinite World Generator
    world_gen_class = unreal.load_class(None, '/Script/MegaregionSim.InfiniteWorldGenerator')
    if world_gen_class:
        world_gen = unreal.EditorLevelLibrary.spawn_actor_from_class(world_gen_class, unreal.Vector(0, 0, 0))
        unreal.log_warning("SPAWNED: Infinite World Chunk Streaming Engine")
    else:
        unreal.log_error("Could not find InfiniteWorldGenerator C++ class!")
        
    # 4. Spawn an AI Train ahead of the player
    ai_train_class = unreal.load_class(None, '/Game/Blueprints/BP_AITrain.BP_AITrain_C')
    if ai_train_class:
        # Spawn AI train 15km ahead on a parallel track offset by 1500 units
        ai_train = unreal.EditorLevelLibrary.spawn_actor_from_class(ai_train_class, unreal.Vector(1500000, 1500, 150))
        unreal.log_warning("SPAWNED: AI Traffic Train")
    else:
        unreal.log_warning("Could not find BP_AITrain. Skipping AI Traffic (Create it manually if you want traffic!)")

    unreal.log("--- READY TO PLAY ---")
    unreal.log_warning("INSTRUCTIONS: Click PLAY to start the infinite simulation!")

start_infinite_sim()
