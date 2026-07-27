#include "GISDataManager.h"
#include "Misc/FileHelper.h"
#include "Math/UnrealMathUtility.h"

bool UGISDataManager::GenerateProceduralHeightmapRAW(int32 Resolution, FString OutputFilePath, float BaseScale, float ZScale, float GhatThreshold)
{
	if (Resolution <= 0) return false;

	// Total elements (16-bit per pixel)
	int32 TotalPixels = Resolution * Resolution;
	TArray<uint16> HeightData;
	HeightData.SetNumUninitialized(TotalPixels);

	// Basic Perlin-style fractal noise generation
	for (int32 y = 0; y < Resolution; ++y)
	{
		for (int32 x = 0; x < Resolution; ++x)
		{
			// Convert coordinates to noise space
			float nx = (float)x * BaseScale;
			float ny = (float)y * BaseScale;

			// Octave 1: Base continent shapes
			float NoiseVal = FMath::PerlinNoise2D(FVector2D(nx, ny)) * 0.5f + 0.5f;
			
			// Octave 2: Mountains
			float MountainNoise = FMath::PerlinNoise2D(FVector2D(nx * 4.0f, ny * 4.0f)) * 0.5f + 0.5f;
			
			// Octave 3: High-frequency ridges (Ghats)
			float RidgeNoise = FMath::PerlinNoise2D(FVector2D(nx * 16.0f, ny * 16.0f));
			RidgeNoise = 1.0f - FMath::Abs(RidgeNoise); // Ridge function (creates sharp peaks/valleys)

			// Combine them intelligently based on game scope
			// If Base is high, it's a mountain range. Add ridges.
			float FinalElevation = NoiseVal * 0.4f + MountainNoise * 0.4f;
			
			if (FinalElevation > GhatThreshold)
			{
				// Add extreme Ghat sections (mountain passes)
				FinalElevation += RidgeNoise * 0.2f;
			}
			else
			{
				// Plains / flatter areas (smooth it out)
				FinalElevation = FMath::Lerp(FinalElevation, 0.2f, 0.5f);
			}

			// Clamp to [0, 1]
			FinalElevation = FMath::Clamp(FinalElevation, 0.0f, 1.0f);

			// Scale to 16-bit integer (0 - 65535)
			uint16 ScaledHeight = (uint16)(FinalElevation * 65535.0f);
			
			HeightData[y * Resolution + x] = ScaledHeight;
		}
	}

	// Write to RAW file (Little Endian uint16 array)
	// Create an array of bytes
	TArray<uint8> ByteData;
	ByteData.SetNumUninitialized(TotalPixels * 2);
	FMemory::Memcpy(ByteData.GetData(), HeightData.GetData(), TotalPixels * 2);

	return FFileHelper::SaveArrayToFile(ByteData, *OutputFilePath);
}
