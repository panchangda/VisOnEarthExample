// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowFieldSettings.h"
#include "FlowFieldResources.h"
#include "CesiumGeoreference.h"


/**
 *
 * For Graphics
 */






class VISONEARTH_API FFlowFieldRenderer
{
public:
	FFlowFieldRenderer();
	~FFlowFieldRenderer();
	void RegisterRendererDelegate();
	void UnregisterRendererDelegate();
	void Render(FPostOpaqueRenderParameters& InParameters);
	
#if FLOWFIELD_USE_COMPUTE
	void AddComputeParticleCSPass(FPostOpaqueRenderParameters& InParameters);
#else
	void AddComputeParticlePSPass(FPostOpaqueRenderParameters& InParameters);
#endif
	
	void AddDrawLinePass(FPostOpaqueRenderParameters& InParameters);
	void AddTestPass(FPostOpaqueRenderParameters& InParameters);
	bool isMultiFrameResourceInitialized();
private:
	FDelegateHandle RenderHandle;
	
	int32 FrameCounter = -1;

	FFlowFieldResources Resources;
	bool isInitializing = false;
	FFlowFieldSettings Settings;
	
	FFlowFieldSettings SettingsFromGame;

	/* GeoReference For Coordinate Transform*/
	ACesiumGeoreference* GeoReference = nullptr;
public:
	
	void UpdateSettings(FFlowFieldSettings NewFlowFieldSettings);
	
	void UpdateGeoReference(ACesiumGeoreference* NewGeoReference);
	
};
