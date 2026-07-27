import unreal
import time

def setup_playtest():
    unreal.log("--- INITIALIZING PHASE 4 INTEGRATION PLAYTEST ---")
    
    # 1. Spawn Weather System
    weather_class = unreal.load_class(None, '/Script/MegaregionSim.MegaregionWeatherSystem')
    if weather_class:
        weather_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(weather_class, unreal.Vector(0,0,1000))
        # Call the C++ function to set weather to Rain!
        if weather_actor:
            weather_actor.set_weather_state(unreal.WeatherState.RAIN)
            unreal.log_warning("SPAWNED: MegaregionWeatherSystem [RAIN MODE ACTIVE]")
    else:
        unreal.log_error("Could not find MegaregionWeatherSystem class! Did you compile?")

    # 2. Spawn 5km Procedural Track Generator (From Phase 1/2)
    track_gen_class = unreal.load_class(None, '/Script/MegaregionSim.TrackGenerator')
    if track_gen_class:
        track_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(track_gen_class, unreal.Vector(0,0,0))
        unreal.log_warning("SPAWNED: Procedural Track Generator (5km)")
    
    # 3. Spawn the TrainPawn!
    train_class = unreal.load_class(None, '/Script/MegaregionSim.TrainPawn')
    if train_class:
        # Spawn slightly above the track to let Chaos physics settle
        train_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(train_class, unreal.Vector(0,0,500))
        unreal.log_warning("SPAWNED: Heavy Freight Locomotive (Physics Engine Active)")
    
    unreal.log("--- SETUP COMPLETE ---")
    unreal.log_warning("INSTRUCTIONS: Click PLAY (PIE). Press 'W' to throttle up.")
    unreal.log_warning("Watch the Output Log to see the Phase 4 VFX and Acoustics proxies reacting to your RPM in real-time!")

if __name__ == "__main__":
    setup_playtest()
