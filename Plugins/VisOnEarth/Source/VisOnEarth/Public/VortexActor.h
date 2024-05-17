// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisOnEarthActor.h"
#include "GameFramework/Actor.h"
#include "VortexActor.generated.h"

UCLASS()
class VISONEARTH_API AVortexActor : public AVisOnEarthActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVortexActor();

	void SpawnSpheresAtLocations(const TArray<FVector>& Locations, UMaterialInterface* Material);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Vortex Rendering")
	UTexture2D* FlowFieldTexture2D;
	
	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	FVector2D LonRange = FVector2D(99, 150);
	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	FVector2D LatRange = FVector2D(-10, 52);
	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	FVector2D LevRange = FVector2D(1000, 1000);
	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	int LongitudeNum = 205;
	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	int LatitudeNum = 249;


	UPROPERTY(EditAnywhere, Category = "Vortex Rendering")
	FVector SphereScaleCoef = FVector(500000.0f);
	
	
	UPROPERTY(EditAnywhere, Category="Vortex Rendering")
	UMaterialInterface* DefaultVortexMaterial;

	
	TArray<FVector2D> VortexPoint;
	TArray<FVector2D> VortexData;
	
	TArray<FVector2D> InitializeData();
	TArray<FVector2D>  DetectVortex(const TArray<FVector2D>& VortexData);

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// Sphere static mesh asset
	UStaticMesh* SphereMeshAsset;
	
	bool IsValidLine(const FVector2D& a, const FVector2D& b, const TArray<FVector2D>& speed);
	bool IsValidQuad(const FVector2D& a, int q, const TArray<FVector2D>& speed);
	TArray<FVector2D> Clockwise(const TArray<FVector2D>& speed);
	TArray<FVector2D> CounterClockWise(const TArray<FVector2D>& speed);
};
