#pragma once
#include "FlowFieldSettings.generated.h"

enum FFlowFieldSettingsChangeType
{
	
};
USTRUCT(BlueprintType)
struct FFlowFieldSettings
{
	GENERATED_USTRUCT_BODY()
	
	/* Field Data Related Settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f DataLonRange = FVector2f(0.0f, 360.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f DataLatRange = FVector2f(-90.0f, 90.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f DataLevRange = FVector2f(10000.0f, 500000.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FIntVector DataDimension = FIntVector(1, 1, 1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector3f SpeedMin = FVector3f(0.0f,0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector3f SpeedMax = FVector3f(0.0f,0.0f, 0.0f);

	/* Camera/View Related Settings*/
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	// FVector2f ViewLonRange = FVector2f(0.0f, 360.0f);
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	// FVector2f ViewLatRange = FVector2f(-90.0f, 90.0f);
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	// FVector2f ViewLevRange = FVector2f(0.0f, 0.0f);


	// For Sietium Genbu02
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f ViewLonRange = FVector2f(50.0f, 200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f ViewLatRange = FVector2f(-90.0f, 90.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	FVector2f ViewLevRange = FVector2f(10000.0f, 500000.0f);

	
	/* Particle Movement Settings */
	// how fast the particles move.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float SpeedScaleFactor = 0.53f;

	// how fast the particle trails fade on each frame.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float TrailFadeOpacity = 0.97f;

	//when the opacity is less than, particles fade out.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float FadeOutOpacity = 0.03f;

	//how often the particles move to a random place.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow Field | Settings")
	float DropRate = 0.03f;

	//drop rate increase relative to individual particle speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow Field | Settings")
	float DropRateFactor = 0.01f;

	// Line Width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	int32 LineWidth = 3.0f;

	// If Draw Arrows For Particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	bool DrawArrow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float ArrowLengthScale = 0.7f;
	
	/* Color Mapping Settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float HueRange = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow Field | Settings")
	float HueOffset = 0.0f;
	
	/* Below Settings could trigger resources update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow Field | Resources Settings")
	FString JsonDataFile = "dataset-omega-3d-rep-weekly_20180103T0000Z_P20200331T0000Z";
	// FString JsonDataFile = "demo";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Flow Field | Resources Settings")
	int32 ParticlesSum = 256*256;
	
	//drop rate increase relative to individual particle speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow Field | Resources Settings")
	int32 TrailLength = 100;

	// 重载等号运算符以比较两个结构体实例
	bool operator==(const FFlowFieldSettings& Other) const
	{
		return DataLonRange == Other.DataLonRange &&
			DataLatRange == Other.DataLatRange &&
			DataLevRange == Other.DataLevRange &&
			DataDimension == Other.DataDimension &&
			SpeedMin == Other.SpeedMin &&
			SpeedMax == Other.SpeedMax &&
			ViewLonRange == Other.ViewLonRange &&
			ViewLatRange == Other.ViewLatRange &&
			ViewLevRange == Other.ViewLevRange &&
			SpeedScaleFactor == Other.SpeedScaleFactor &&
			TrailFadeOpacity == Other.TrailFadeOpacity &&
			FadeOutOpacity == Other.FadeOutOpacity &&
			DropRate == Other.DropRate &&
			DropRateFactor == Other.DropRateFactor &&
			LineWidth == Other.LineWidth &&
			DrawArrow == Other.DrawArrow &&
			HueRange == Other.HueRange &&
			HueOffset == Other.HueOffset &&
			JsonDataFile == Other.JsonDataFile &&
			ParticlesSum == Other.ParticlesSum &&
			TrailLength == Other.TrailLength;
	}
};


BEGIN_SHADER_PARAMETER_STRUCT(FlowFieldSettingsParameters, )
SHADER_PARAMETER(FVector2f, DataLonRange)
SHADER_PARAMETER(FVector2f, DataLatRange)
SHADER_PARAMETER(FVector2f, DataLevRange)
SHADER_PARAMETER(FIntVector3, DataDimension)
SHADER_PARAMETER(FVector2f, ViewLonRange)
SHADER_PARAMETER(FVector2f, ViewLatRange)
SHADER_PARAMETER(FVector2f, ViewLevRange)
SHADER_PARAMETER(float, SpeedScaleFactor)
SHADER_PARAMETER(float, TrailFadeOpacity)
SHADER_PARAMETER(float, DropRate)
SHADER_PARAMETER(float, DropRateFactor)
SHADER_PARAMETER(float, HueRange)
SHADER_PARAMETER(float, HueOffset)
SHADER_PARAMETER(uint32, ParticleSum)
SHADER_PARAMETER(uint32, TrailLength)
END_SHADER_PARAMETER_STRUCT()

static void FillFlowFieldSettingsParameters(FlowFieldSettingsParameters& FlowFieldSettingsParameters, const FFlowFieldSettings& InFlowFieldSettings)
{
	FlowFieldSettingsParameters.DataLonRange = InFlowFieldSettings.DataLonRange;
	FlowFieldSettingsParameters.DataLatRange = InFlowFieldSettings.DataLatRange;
	FlowFieldSettingsParameters.DataLevRange = InFlowFieldSettings.DataLevRange;
	FlowFieldSettingsParameters.DataDimension = InFlowFieldSettings.DataDimension;
	FlowFieldSettingsParameters.ViewLonRange = InFlowFieldSettings.ViewLonRange;
	FlowFieldSettingsParameters.ViewLatRange = InFlowFieldSettings.ViewLatRange;
	FlowFieldSettingsParameters.ViewLevRange = InFlowFieldSettings.ViewLevRange;
	FlowFieldSettingsParameters.SpeedScaleFactor = InFlowFieldSettings.SpeedScaleFactor;
	FlowFieldSettingsParameters.TrailFadeOpacity = InFlowFieldSettings.TrailFadeOpacity;
	FlowFieldSettingsParameters.DropRate = InFlowFieldSettings.DropRate;
	FlowFieldSettingsParameters.DropRateFactor = InFlowFieldSettings.DropRateFactor;
	FlowFieldSettingsParameters.HueRange = InFlowFieldSettings.HueRange;
	FlowFieldSettingsParameters.HueOffset = InFlowFieldSettings.HueOffset;
	FlowFieldSettingsParameters.ParticleSum = InFlowFieldSettings.ParticlesSum;
	FlowFieldSettingsParameters.TrailLength = InFlowFieldSettings.TrailLength;
}

static FlowFieldSettingsParameters GetFlowFieldSettingsParameters(const FFlowFieldSettings& InFlowFieldSettings)
{
	FlowFieldSettingsParameters Parameters;
	Parameters.DataLonRange = InFlowFieldSettings.DataLonRange;
	Parameters.DataLatRange = InFlowFieldSettings.DataLatRange;
	Parameters.DataLevRange = InFlowFieldSettings.DataLevRange;
	Parameters.DataDimension = InFlowFieldSettings.DataDimension;
	Parameters.ViewLonRange = InFlowFieldSettings.ViewLonRange;
	Parameters.ViewLatRange = InFlowFieldSettings.ViewLatRange;
	Parameters.ViewLevRange = InFlowFieldSettings.ViewLevRange;
	Parameters.SpeedScaleFactor = InFlowFieldSettings.SpeedScaleFactor;
	Parameters.TrailFadeOpacity = InFlowFieldSettings.TrailFadeOpacity;
	Parameters.DropRate = InFlowFieldSettings.DropRate;
	Parameters.DropRateFactor = InFlowFieldSettings.DropRateFactor;
	Parameters.HueRange = InFlowFieldSettings.HueRange;
	Parameters.HueOffset = InFlowFieldSettings.HueOffset;
	Parameters.ParticleSum = InFlowFieldSettings.ParticlesSum;
	Parameters.TrailLength = InFlowFieldSettings.TrailLength;
	return Parameters;
	
}