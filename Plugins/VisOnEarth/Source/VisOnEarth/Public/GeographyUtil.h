
#pragma once



inline FVector NormalizedToRanged(
	FVector2D LonRange, FVector2D LatRange, FVector2D LevRange,
	FVector InVector)
{
	return FVector(InVector.X*(LonRange.Y - LonRange.X) + LonRange.X,
		InVector.Y*(LatRange.Y - LatRange.X) + LatRange.X,
		InVector.Z*(LevRange.Y - LevRange.X) + LevRange.X);
}