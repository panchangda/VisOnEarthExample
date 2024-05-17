#pragma once

#include "CoreMinimal.h"
#include "VisOnEarthRenderer.generated.h"


UCLASS()
class UVisOnEarthRenderer : public UObject
{
	GENERATED_BODY()
public:
	 UVisOnEarthRenderer(){}
	virtual ~UVisOnEarthRenderer(){}
	void RegisterRendererDelegate();
	void UnregisterRendererDelegate();
	virtual void Render(FPostOpaqueRenderParameters& InParameters){}
	
	
	FString RendererName = "NullRenderer";
	
private:
	FDelegateHandle RenderHandle;

};
