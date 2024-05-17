// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisOnEarthActor.generated.h"

class UVisOnEarthRenderer;
class ACesiumGeoreference;

UCLASS()
class VISONEARTH_API AVisOnEarthActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVisOnEarthActor();

	// Renderer
	UPROPERTY(EditAnywhere, Category = "VisOnEarth Rendering")
	TSubclassOf<UVisOnEarthRenderer> RendererClass;
	void RegisterRenderer();
	void UnregisterRenderer();


	// Geography
	UPROPERTY(EditAnywhere, Category="VisOnEarth Rendering")
	FName DefaultGeoReferenceTag = FName("DEFAULT_GEOREFERENCE");
	UPROPERTY(EditAnywhere, Category="VisOnEarth Rendering")
	ACesiumGeoreference* DefaultGeoReference = nullptr;
	
	ACesiumGeoreference* FindTaggedGeoReference(const FName& Tag);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	TObjectPtr<UVisOnEarthRenderer> Renderer;
};
