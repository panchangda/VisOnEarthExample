// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LICRenderer.h"
#include "VisOnEarthActor.h"
#include "LICActor.generated.h"

class UProceduralMeshComponent;
class ACesiumGeoreference;

UCLASS()
class VISONEARTH_API ALICActor : public AVisOnEarthActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALICActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	UPROPERTY(VisibleAnywhere, Category="LIC Rendering")
	UTexture2D* FlowFieldTexture2D;

	UPROPERTY(VisibleAnywhere, Category="LIC Rendering")
	UTexture2D* NoiseTexture2D;
	
	// 材质属性
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	UMaterialInterface* LICMeshMaterial;

	void ApplyMaterialOnMesh();

	
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* LICMesh;
	
	void GenerateMeshOnEarth();

	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	int MeshRowVerticesNum = 360;
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	int MeshColVerticesNum = 180;
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	FVector2D LonRange = FVector2D(99, 150);
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	FVector2D LatRange = FVector2D(-10, 52);
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	FVector2D LevRange = FVector2D(100000, 100000);
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	int LongitudeNum;
	UPROPERTY(EditAnywhere, Category = "LIC Rendering")
	int LatitudeNum;
	
	void SaveFlowFieldTexture2DToAsset();

private:

	void InitializeFlowFieldTexture2D();
	void InitializeNoiseTexture2D();
	
};
