
# Core Architecture Rules
1. NO Hardcoded Soft/Hard Object Paths:
   - Never hardcode ConstructorHelpers::FObjectFinder with direct string paths to .uasset files inside C++.
   - Use TSubclassOf<AActor>, TSoftObjectPtr<UStaticMesh>, or dedicated UDataAsset properties marked with UPROPERTY(EditDefaultsOnly, Category = "Assets") so assets can be assigned in the Editor or Data Assets.

2. Spawning & Object Lifetime:
   - Always validate pointers with IsValid() or IF_VALID check before accessing spawned actors or components.
   - Use FActorSpawnParameters explicitly (specify Owner, Instigator, SpawnCollisionHandlingMethod).
   - Store spawned actors in UPROPERTY() macro-tracked containers (TArray<TObjectPtr<AActor>>) to prevent Garbage Collection issues.

3. Alignment & Grid / Socket Snapping:
   - For train track / building alignment, use Transform Matrix math (FTransform), Sockets (GetSocketTransform), or Grid Snapping logic (FMath::GridSnap).
   - Do NOT perform raw, unaligned vector additions for snapping building modules or track pieces.
   - Use Attachment Rules (FAttachmentTransformRules::SnapToTargetNotIncludingScale or KeepWorld).

4. Component vs. Actor Separation:
   - Modular building pieces should ideally be UStaticMeshComponent / UInstancedStaticMeshComponent inside a single grid manager actor when high performance is needed, OR clean actor components (UActorComponent) attached to a parent builder actor.

5. Naming Conventions:
   - Strictly follow Epic standards: A prefix for Actors, U for Objects/Components, F for Structs, E for Enums, b prefix for Booleans (e.g., bIsTrackConnected).
   - Use UPROPERTY() and UFUNCTION() macros with proper specifiers (BlueprintReadOnly, BlueprintCallable, Category).

6. Zero Manual Configuration:
   - Provide solutions that can be entirely driven via code or automated scripts.
   - Do NOT propose architectures that require the user to manually open the Unreal Engine Editor, dive into Blueprint Details panels, or perform manual UI-based configurations unless explicitly asked.
