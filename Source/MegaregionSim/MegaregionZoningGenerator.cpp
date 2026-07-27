#include "MegaregionZoningGenerator.h"
#include "Math/UnrealMathUtility.h"

// Simple pseudo-random hash based on coordinates
float Hash2D(FVector2D p, int32 Seed)
{
	p = FVector2D(FVector2D::DotProduct(p, FVector2D(127.1, 311.7)), FVector2D::DotProduct(p, FVector2D(269.5, 183.3)));
	float s = FMath::Sin(p.X + Seed) * 43758.5453123f;
	return s - FMath::FloorToFloat(s);
}

EZoningClassification UMegaregionZoningGenerator::GetZoningAtLocation(FVector2D Location, int32 RandomSeed, float Scale)
{
	FVector2D ScaledLoc = Location * Scale;
	
	// Cellular noise (Voronoi) to determine distance to nearest city center
	FVector2D Cell = FVector2D(FMath::FloorToFloat(ScaledLoc.X), FMath::FloorToFloat(ScaledLoc.Y));
	FVector2D Fract = FVector2D(ScaledLoc.X - Cell.X, ScaledLoc.Y - Cell.Y);

	float MinDist = 999999.0f;
	FVector2D NearestSeed = FVector2D::ZeroVector;
	float SecondMinDist = 999999.0f;

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			FVector2D Neighbor = FVector2D(x, y);
			FVector2D PointOffset;
			PointOffset.X = Hash2D(Cell + Neighbor, RandomSeed);
			PointOffset.Y = Hash2D(Cell + Neighbor, RandomSeed + 1337);

			FVector2D Diff = Neighbor + PointOffset - Fract;
			float Dist = Diff.Size();

			if (Dist < MinDist)
			{
				SecondMinDist = MinDist;
				MinDist = Dist;
				NearestSeed = Cell + Neighbor;
			}
			else if (Dist < SecondMinDist)
			{
				SecondMinDist = Dist;
			}
		}
	}

	// Use the distance to the city center (MinDist) to zone the area
	
	// If it's a massive city cluster (determined by hash of the seed)
	float CityTypeHash = Hash2D(NearestSeed, RandomSeed + 42);

	if (CityTypeHash > 0.8f) 
	{
		// Major Metropolis (Rare)
		if (MinDist < 0.1f) return EZoningClassification::UrbanCenter;
		if (MinDist < 0.25f) return EZoningClassification::Suburbs;
		if (MinDist < 0.4f) return EZoningClassification::HeavyIndustrial;
	}
	else if (CityTypeHash > 0.4f)
	{
		// Medium Industrial Town
		if (MinDist < 0.15f) return EZoningClassification::HeavyIndustrial;
		if (MinDist < 0.3f) return EZoningClassification::Suburbs;
	}
	else
	{
		// Small Remote Village
		if (MinDist < 0.05f) return EZoningClassification::Village;
	}

	return EZoningClassification::Nature;
}
