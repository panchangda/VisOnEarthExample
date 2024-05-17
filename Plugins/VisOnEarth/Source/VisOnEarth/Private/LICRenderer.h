#pragma once

#include "CoreMinimal.h"
#include "VisOnEarthRenderer.h"
#include "LICRenderer.generated.h"

UCLASS()
class ULICRenderer : public UVisOnEarthRenderer
{
	GENERATED_BODY()
public:

	ULICRenderer();

	virtual void Render(FPostOpaqueRenderParameters& InParameters) override;


	bool isInitializing;
	FTexture2DRHIRef FlowFieldTexture2D = nullptr;
	void InitializeFlowFieldData();
	
};
