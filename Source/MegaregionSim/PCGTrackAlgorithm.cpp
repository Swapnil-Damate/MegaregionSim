#include "PCGTrackAlgorithm.h"
#include "Math/UnrealMathUtility.h"

TArray<FTrackPointData> UPCGTrackAlgorithm::ProcessTrackPath(const TArray<FVector>& RawPathPoints, float StandardMaxGrade, float GhatMaxGrade, float TunnelDepthThreshold, float BridgeHeightThreshold)
{
	TArray<FTrackPointData> ProcessedData;

	if (RawPathPoints.Num() < 2)
	{
		return ProcessedData;
	}

	FVector CurrentPos = RawPathPoints[0];
	
	for (int32 i = 0; i < RawPathPoints.Num(); ++i)
	{
		FTrackPointData PointData;
		FVector TargetPos = RawPathPoints[i];

		if (i > 0)
		{
			FVector Direction = TargetPos - CurrentPos;
			float Distance2D = FVector2D(Direction.X, Direction.Y).Size();
			float HeightDiff = Direction.Z;

			if (Distance2D > 0.0f)
			{
				// Calculate Grade (%)
				float Grade = (HeightDiff / Distance2D) * 100.0f;
				PointData.Grade = Grade;

				// Is it a Ghat section? (Grade > StandardMax)
				if (FMath::Abs(Grade) > StandardMaxGrade)
				{
					PointData.bIsGhatSection = true;

					// Enforce Max Ghat Grade (4%)
					if (Grade > GhatMaxGrade)
					{
						// Too steep! Cut into the mountain (Tunnel) or clamp
						float MaxAllowedZ = CurrentPos.Z + (Distance2D * (GhatMaxGrade / 100.0f));
						
						// If we have to dig deep to meet the grade, it's a tunnel
						if (TargetPos.Z - MaxAllowedZ > TunnelDepthThreshold)
						{
							PointData.bIsTunnel = true;
						}
						
						TargetPos.Z = MaxAllowedZ;
					}
					else if (Grade < -GhatMaxGrade)
					{
						// Too steep down! Build a bridge over the valley or clamp
						float MinAllowedZ = CurrentPos.Z - (Distance2D * (GhatMaxGrade / 100.0f));
						
						// If the ground drops away too fast, build a bridge
						if (MinAllowedZ - TargetPos.Z > BridgeHeightThreshold)
						{
							PointData.bIsBridge = true;
						}
						
						TargetPos.Z = MinAllowedZ;
					}
				}

				// Superelevation (Banking) Logic
				// If we have a next point, calculate curve radius and apply roll
				if (i < RawPathPoints.Num() - 1)
				{
					FVector NextDir = (RawPathPoints[i+1] - TargetPos).GetSafeNormal2D();
					FVector CurrentDir = Direction.GetSafeNormal2D();
					
					// Cross product Z gives the turn direction (-1 to 1)
					float TurnSharpness = FVector::CrossProduct(CurrentDir, NextDir).Z;
					
					// Apply Roll (Banking) based on turn sharpness.
					// Max bank angle is roughly 10 degrees for heavy freight to prevent tipping.
					float BankAngle = TurnSharpness * 10.0f;
					
					PointData.Rotation = CurrentDir.Rotation();
					PointData.Rotation.Roll = BankAngle;
				}
				else
				{
					PointData.Rotation = Direction.Rotation();
				}
			}
		}
		else
		{
			// First point
			PointData.Rotation = (RawPathPoints[1] - TargetPos).Rotation();
		}

		PointData.Location = TargetPos;
		ProcessedData.Add(PointData);

		// Move forward
		CurrentPos = TargetPos;
	}

	return ProcessedData;
}
