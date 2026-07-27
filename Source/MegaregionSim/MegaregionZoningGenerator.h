#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MegaregionZoningGenerator.generated.h"

UENUM(BlueprintType)
enum class EZoningClassification : uint8
{
	Nature,
	Village,
	Suburbs,
	UrbanCenter,
	HeavyIndustrial
};

/**
 * Procedural Zoning algorithm for defining massive scale city boundaries.
 * Uses a form of cellular automata and spatial hashing.
 */
UCLASS()
class MEGAREGIONSIM_API UMegaregionZoningGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Determines the zoning classification for a specific X, Y coordinate in the Megaregion.
	 * Uses deterministic Voronoi noise to spawn "Seed Cities" and gradient falloffs for suburbs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Megaregion|PCG")
	static EZoningClassification GetZoningAtLocation(FVector2D Location, int32 RandomSeed = 42, float Scale = 0.0001f);
};
