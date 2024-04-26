// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CesiumGeoreference.h"
#include "FlowFieldSettings.h"
#include "FlowFieldRenderer.h"
#include "FlowFieldActor.generated.h"

class UFlowFieldUIWidget;

UCLASS()
class VISONEARTH_API AFlowFieldActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlowFieldActor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/* GeoReference For GIS Transform */
	UPROPERTY(EditAnywhere, Category="Flow Field Rendering")
	ACesiumGeoreference* GeoReference;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flow Field Rendering")
	FFlowFieldSettings FlowFieldSettings;
	
	// Update Self Settings and Renderer Settings
	UFUNCTION(BlueprintCallable, Category= "Flow Field Rendering")
	void UpdateFlowFieldSettings(const FFlowFieldSettings& NewFlowFieldSettings);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	/* Flow Field Renderer */
	TSharedPtr<FFlowFieldRenderer> renderer;
	void RegisterRenderer();
	void UnregisterRenderer();
	void UpdateRendererSettings();

	ACesiumGeoreference* FindTaggedGeoReference(const FName& Tag);
	void UpdateRendererGeoReference() const;
};

