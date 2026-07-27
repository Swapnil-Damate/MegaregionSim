import unreal

def setup_complete_simulation():
    unreal.log("--- INITIALIZING COMPLETE MEGAREGION SIMULATION ---")
    
    # 1. Spawn Master Controller (This generates PCG mountains, grass, and cities!)
    master_class = unreal.load_class(None, '/Script/MegaregionSim.MasterSimulationController')
    if master_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(master_class, unreal.Vector(0,0,0))
        unreal.log_warning("SPAWNED: MasterSimulationController (PCG World Generating...)")
    else:
        unreal.log_error("Could not find MasterSimulationController! Did you compile?")

    # 2. Spawn Weather System
    weather_class = unreal.load_class(None, '/Script/MegaregionSim.MegaregionWeatherSystem')
    if weather_class:
        weather_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(weather_class, unreal.Vector(0,0,1000))
        if weather_actor:
            weather_actor.set_weather_state(unreal.WeatherState.RAIN)
            unreal.log_warning("SPAWNED: MegaregionWeatherSystem [RAIN MODE ACTIVE]")

    # 3. Spawn the TrainPawn! (Spawning far outside the city grid to prevent spawning inside a skyscraper)
    train_class = unreal.load_class(None, '/Script/MegaregionSim.TrainPawn')
    if train_class:
        train_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(train_class, unreal.Vector(0,-30000,500))
        unreal.log_warning("SPAWNED: Heavy Freight Locomotive & Consist")
    
    unreal.log("--- MEGAREGION SETUP COMPLETE ---")
    unreal.log_warning("INSTRUCTIONS: Click PLAY (PIE).")

setup_complete_simulation()
