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
struct FTriVertexForInstanceDraw
{
	FVector4f Vertex;
};
class FTriVertexForInstanceDrawDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	FTriVertexForInstanceDrawDeclaration() = default;
	FTriVertexForInstanceDrawDeclaration(FTriVertexForInstanceDrawDeclaration&&) = default;
	virtual ~FTriVertexForInstanceDrawDeclaration() = default;

	void virtual InitRHI(FRHICommandListBase& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		uint16 Stride = sizeof(FTriVertexForInstanceDraw);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTriVertexForInstanceDraw, Vertex), VET_Float4, 0, Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	}

	void virtual ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};





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
	
	bool isMultiFrameResourceInitialized();
private:
	FDelegateHandle RenderHandle;
	
	int32 FrameCounter = -1;

	FFlowFieldResources Resources;
	
	FFlowFieldSettings Settings;
	
	FFlowFieldSettings SettingsFromGame;

	/* GeoReference For Coordinate Transform*/
	ACesiumGeoreference* GeoReference = nullptr;
public:
	
	void UpdateSettings(FFlowFieldSettings NewFlowFieldSettings);
	
	void UpdateGeoReference(ACesiumGeoreference* NewGeoReference);
	
};
