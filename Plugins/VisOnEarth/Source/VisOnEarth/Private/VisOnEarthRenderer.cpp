#include "VisOnEarthRenderer.h"

#include "EngineModule.h"

void UVisOnEarthRenderer::RegisterRendererDelegate()
{
	check(IsInRenderingThread())

	if(RenderHandle.IsValid()) return ;

	RenderHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateUObject(this, &UVisOnEarthRenderer::Render));

	UE_LOG(LogTemp, Warning,  TEXT("%s is Registered!"), *RendererName);
}

void UVisOnEarthRenderer::UnregisterRendererDelegate()
{

	if(!RenderHandle.IsValid()) return;

	GetRendererModule().RemovePostOpaqueRenderDelegate(RenderHandle);
	RenderHandle.Reset();

	UE_LOG(LogTemp, Warning, TEXT("%s is Destroyed!"), *RendererName);
}
