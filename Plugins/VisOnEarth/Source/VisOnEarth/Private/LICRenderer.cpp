#include "LICRenderer.h"

#include "Interfaces/IPluginManager.h"

ULICRenderer::ULICRenderer()
{
	this->RendererName = "LICRenderer";
}

void ULICRenderer::Render(FPostOpaqueRenderParameters& InParameters)
{
	// UE_LOG(LogTemp, Log, TEXT("LICRenderer's render function is called"));
	
	if(!FlowFieldTexture2D.IsValid())
	{
		if(isInitializing) return;
		
		isInitializing = true;	
	}
}

void ULICRenderer::InitializeFlowFieldData()
{

}



