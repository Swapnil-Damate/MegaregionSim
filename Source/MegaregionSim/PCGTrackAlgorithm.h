#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PCGTrackAlgorithm.generated.h"

USTRUCT(BlueprintType)
struct FTrackPointData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	float Grade = 0.0f; // Grade in percentage (e.g., 2.0 = 2%)

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	bool bIsTunnel = false;

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	bool bIsBridge = false;

	UPROPERTY(BlueprintReadWrite, Category = "Megaregion|PCG")
	bool bIsGhatSection = false;
};

/**
 * Mathematical rule engine for massive-scale procedural track laying.
 * Enforces strict simulation rules like 2% standard grades, 4% Ghat grades, and calculates superelevation.
 */
UCLASS()
class MEGAREGIONSIM_API UPCGTrackAlgorithm : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Takes a raw array of target coordinates (e.g., from a Spline or A* pathfinder)
	 * and applies hardcore rail engineering rules (Grade limits, Banking, Cut/Fill).
	 */
	UFUNCTION(BlueprintCallable, Category = "Megaregion|PCG")
	static TArray<FTrackPointData> ProcessTrackPath(const TArray<FVector>& RawPathPoints, float StandardMaxGrade = 2.0f, float GhatMaxGrade = 4.0f, float TunnelDepthThreshold = 500.0f, float BridgeHeightThreshold = 500.0f);
};
