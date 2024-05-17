// Fill out your copyright notice in the Description page of Project Settings.


#include "LICActor.h"

#include "CesiumGeoreference.h"
#include "EngineUtils.h"
#include "LICRenderer.h"
#include "Interfaces/IPluginManager.h"
#include "EncodeUTextureUtils.h"
#include "ProceduralMeshComponent.h"
#include "GeographyUtil.h"
#include "DataIOUtil.h"


// Sets default values
ALICActor::ALICActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	LICMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = LICMesh;

}

void ALICActor::BeginPlay()
{
	Super::BeginPlay();
	InitializeFlowFieldTexture2D();
	InitializeNoiseTexture2D();
	GenerateMeshOnEarth();
	ApplyMaterialOnMesh();
	UE_LOG(LogTemp, Warning, TEXT("LIC Actor is Initialized!") );
}

void ALICActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("LIC Actor is Destoyed!") );
}

// Called every frame
void ALICActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ALICActor::GenerateMeshOnEarth()
{

	TArray<FVector>vertices;
    TArray<int32> triangles;
    TArray<FVector2D> UVs;
	
	int Rows = MeshRowVerticesNum;
	int Columns = MeshColVerticesNum;
	float UStep = 1.0f / Rows;
	float VStep = 1.0f / Columns;

	// 创建顶点和UV坐标
	for (int32 i = 0; i <= Rows; ++i)
	{
		for (int32 j = 0; j <= Columns; ++j)
		{
			float x = j / (float) Columns;
			float y = i / (float) Rows;
			vertices.Add(FVector(x, y, 0));
			UVs.Add(FVector2D(j / (float)Columns, i / (float)Rows));
		}
	}

	// 创建索引
	for (int32 i = 0; i < Rows; ++i)
	{
		for (int32 j = 0; j < Columns; ++j)
		{
			int32 TopLeft = i * (Columns + 1) + j;
			int32 TopRight = TopLeft + 1;
			int32 BottomLeft = (i + 1) * (Columns + 1) + j;
			int32 BottomRight = BottomLeft + 1;
			
			// Default is CW, CCW triangles will be culled!
			triangles.Add(TopLeft);
			triangles.Add(TopRight);
			triangles.Add(BottomLeft);
			
			triangles.Add(TopRight);
			triangles.Add(BottomRight);
			triangles.Add(BottomLeft);
			 
		}
	}
	

	TArray<FVector> GeoTransformedVertices;
	GeoTransformedVertices.SetNum(vertices.Num());
	

	for (int i = 0; i < vertices.Num(); i++)
	{
		GeoTransformedVertices[i] = NormalizedToRanged(LonRange, LatRange, LevRange, vertices[i]);
		if (IsValid(DefaultGeoReference))
		{
			GeoTransformedVertices[i] = DefaultGeoReference->TransformLongitudeLatitudeHeightPositionToUnreal(GeoTransformedVertices[i]);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Cannot Transform LIC MESH Vertices From LLH to UE Local Space"));
		}
	}
	

    TArray<FVector> normals;
    TArray<FLinearColor> vertexColors;
    TArray<FProcMeshTangent> tangents;

	// update in 
	// LICMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// LICMesh->UpdateMeshSection_LinearColor(0, vertices, normals, UVs, vertexColors, tangents, false);

	// Create the mesh section
    LICMesh->CreateMeshSection_LinearColor(0, GeoTransformedVertices, triangles, normals, UVs, vertexColors, tangents, false);

	

}

void ALICActor::ApplyMaterialOnMesh()
{
	if (LICMeshMaterial)
	{
	    LICMesh->SetMaterial(0, LICMeshMaterial);
	}else
	{
		UE_LOG(LogTemp, Log, TEXT("LIC Material not Set!"));
	}
}


void ALICActor::InitializeFlowFieldTexture2D()
{

	// Load U.DAT & V.DAT as flowfield uv value
	FString UDataFileName = "U.DAT";
	FString VDataFileName = "V.DAT";
	
	FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("VisOnEarth"))->GetBaseDir();
	FString UDataFilePath = FPaths::Combine(*PluginDir, FString::Format(TEXT("Resources/NCData/{0}"), {UDataFileName}));
	FString VDataFilePath = FPaths::Combine(*PluginDir, FString::Format(TEXT("Resources/NCData/{0}"), {VDataFileName}));
	
	TArray<float> UValues, VValues, WValues;
	
	UValues = LoadDatFile(UDataFilePath, LongitudeNum, LatitudeNum);
	VValues = LoadDatFile(VDataFilePath, LongitudeNum, LatitudeNum);
	WValues.SetNumZeroed(LongitudeNum * LatitudeNum);

	FlowFieldTexture2D = ConvertFloat32ToUTexture2D(UValues, VValues, WValues, LongitudeNum, LatitudeNum);
	

	// Create And Save UTexture to Asset
	// FString TextureName = "LICTexture";
	// CreateAndSaveTextureToAsset(TextureName, UValues, VValues, WValues, LongitudeNum, LatitudeNum);
}


void ALICActor::InitializeNoiseTexture2D()
{
	
	TArray<float> noiseValues;
	noiseValues.SetNum(LongitudeNum * LatitudeNum);
	for(int i =0 ;i < noiseValues.Num();i++)
	{
		noiseValues[i] = FMath::FRandRange(0.0, 1.0);
	}

	NoiseTexture2D = ConvertFloat32ToUTexture2D(noiseValues, noiseValues, noiseValues, LongitudeNum, LatitudeNum);

	// FString TextureName = "NoiseTexture";
	// CreateAndSaveTextureToAsset(TextureName, noiseValues, noiseValues, noiseValues, LongitudeNum, LatitudeNum);
}



void ALICActor::SaveFlowFieldTexture2DToAsset()
{
	
	// FString PackageName = "/VisOnEarth/LICTexture";
	// SaveUTextureToAsset(PackageName, FlowFieldTexture2D);
}

