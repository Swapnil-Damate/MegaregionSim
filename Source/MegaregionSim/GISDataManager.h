#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GISDataManager.generated.h"

/**
 * Subsystem for generating procedural terrain heightmaps (Phase 3.1)
 * Used to create raw mathematical heightmaps (DEMs) for Unreal Landscape import.
 */
UCLASS()
class MEGAREGIONSIM_API UGISDataManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Generates a 16-bit RAW heightmap using fractal Perlin noise.
	 * @param Resolution The resolution of the heightmap (e.g., 2017x2017 or 4033x4033)
	 * @param OutputFilePath Absolute path to save the .raw file
	 * @param BaseScale The base frequency of the noise (lower = larger mountains)
	 * @param ZScale The maximum height of the mountains
	 * @param GhatThreshold Used to determine steep 'Ghat' (mountain pass) sections
	 * @return True if successfully written to disk.
	 */
	UFUNCTION(BlueprintCallable, Category = "Megaregion|GIS")
	static bool GenerateProceduralHeightmapRAW(int32 Resolution, FString OutputFilePath, float BaseScale = 0.001f, float ZScale = 65535.0f, float GhatThreshold = 0.7f);
};
