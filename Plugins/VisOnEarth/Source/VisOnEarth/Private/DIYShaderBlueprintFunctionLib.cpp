#include "DIYShaderBlueprintFunctionLib.h"

#include "CopyTextureShaders.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GlobalShader.h"
#include "PipelineStateCache.h"
#include "RHIStaticStates.h"
#include "RHIResources.h"
#include "SceneInterface.h"
#include "ShaderParameterUtils.h"
#include "Logging/MessageLog.h"
#include "Internationalization/Internationalization.h"
#include "Runtime/RenderCore/Public/RenderTargetPool.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "PixelShaderUtils.h"
#include "Experimental/Graph/GraphConvert.h"


#define LOCTEXT_NAMESPACE "UDIYShaderBlueprintFunctionLib"


// Define Simple Uniform Buffer
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FSimpleUniformStructParameters,)
	SHADER_PARAMETER(FVector4f, Color1)
	SHADER_PARAMETER(FVector4f, Color2)
	SHADER_PARAMETER(FVector4f, Color3)
	SHADER_PARAMETER(FVector4f, Color4)
	SHADER_PARAMETER(uint32, ColorIndex)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

/*
	Define Shader Class
*/
class FDIYShader : public FGlobalShader
{
public:
	FDIYShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
	{
		BindForLegacyShaderParameters<FParameters>(this, Initializer.PermutationId, Initializer.ParameterMap, true);
	}

	FDIYShader()
	{
	}

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_REF(FSimpleUniformStructParameters, SimpleUniformStruct)
		SHADER_PARAMETER_TEXTURE(Texture2D, TextureVal)
		SHADER_PARAMETER_SAMPLER(SamplerState, TextureSampler)
		SHADER_PARAMETER(FVector4f, SimpleColor)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
		//return true;
	}
};

class FDIYShaderVS : public FDIYShader
{
	DECLARE_SHADER_TYPE(FDIYShaderVS, Global);

public:
	FDIYShaderVS()
	{
	}

	FDIYShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDIYShader(Initializer)
	{
	}
};

class FDIYShaderPS : public FDIYShader
{
	DECLARE_SHADER_TYPE(FDIYShaderPS, Global);

public:
	FDIYShaderPS()
	{
	}

	FDIYShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDIYShader(Initializer)
	{
	}
};

class FDIYShaderCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FDIYShaderCS);
	SHADER_USE_PARAMETER_STRUCT(FDIYShaderCS, FGlobalShader)

public:
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InRenderTarget)
		SHADER_PARAMETER_SAMPLER(SamplerState, InRTSampler)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutUAV)
	END_SHADER_PARAMETER_STRUCT()
};


IMPLEMENT_SHADER_TYPE(, FDIYShaderCS, TEXT("/Plugin/VisOnEarth/Private/WriteColorCS.usf"), TEXT("MainCS"), SF_Compute);


// Declare Simple Uniform Buffer
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FSimpleUniformStructParameters, "SimpleUniformStruct");

/* 
	Shader Class & 
	Shader Source File & 
	Function Name(Main Function) &
	Frequency(Shader Type) 
*/
// MainVS Copied From FullscreenVertexShader.usf
IMPLEMENT_SHADER_TYPE(, FDIYShaderVS, TEXT("/Plugin/VisOnEarth/Private/DrawColorShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FDIYShaderPS, TEXT("/Plugin/VisOnEarth/Private/DrawColorShader.usf"), TEXT("MainPS"), SF_Pixel);


struct FSimpleVertex
{
public:
	FVector4 Position;
	//FVector2D UV;
};

//static void DrawColorBoard_RenderThread(
//	FRHICommandListImmediate& RHICmdList,
//	FTextureRenderTargetResource* OutTextureRenderTargetResource,
//	FLinearColor Color,
//	ERHIFeatureLevel::Type FeatureLevel
//)
//{
//check(IsInRenderingThread());

//SCOPED_DRAW_EVENT(RHICmdList, DrawColorBoard);

//FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();

//FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::Clear_Store);
//RHICmdList.BeginRenderPass(RPInfo, TEXT("DIYShaderPass"));
//{

//	// Update viewport.
//	const FIntPoint outMapResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());
//	RHICmdList.SetViewport(0, 0, 0.f, outMapResolution.X, outMapResolution.Y, 1.f);

//	// Get shaders.
//	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
//	TShaderMapRef<FDIYShaderVS> VertexShader(GlobalShaderMap);
//	TShaderMapRef<FDIYShaderPS> PixelShader(GlobalShaderMap);
//	
//	// Set the graphic pipeline state.
//	FGraphicsPipelineStateInitializer GraphicsPSOInit;
//	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
//	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
//	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
//	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
//	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
//	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
//	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
//	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
//	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

//	PixelShader->SetParameters(RHICmdList, Color);
//	
//	//FPixelShaderUtils::DrawFullscreenTriangle(RHICmdList, 1);
//	RHICmdList.SetStreamSource(0, GScreenRectangleVertexBuffer.VertexBufferRHI, 0);
//	RHICmdList.DrawPrimitive(0, 2, 1);
//}
//RHICmdList.EndRenderPass();
//TransitionAndCopyTexture(RHICmdList, OutTextureRenderTargetResource->GetRenderTargetTexture(), OutTextureRenderTargetResource->TextureRHI, {});


//}

static void RDGDraw(
	FRHICommandListImmediate& RHICmdList,
	FTexture2DRHIRef RenderTargetRHI,
	FSimpleShaderParameter InParameter,
	FTexture2DRHIRef InTextureRHI,
	FLinearColor InColor,
	ERHIFeatureLevel::Type FeatureLevel
)
{
	check(IsInRenderingThread());
	
	FRDGBuilder GraphBuilder(RHICmdList);
	// Create RT For RDGPass (necessary when ERDGPassFlags::Raster)
	const FRDGTextureDesc& RenderTargetDesc = FRDGTextureDesc::Create2D(RenderTargetRHI->GetSizeXY(),
	                                                                    RenderTargetRHI->GetFormat(),
	                                                                    FClearValueBinding::Black,
	                                                                    TexCreate_RenderTargetable |
	                                                                    TexCreate_ShaderResource | TexCreate_UAV);
	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
	FRDGTextureRef RDGRenderTarget = GraphBuilder.CreateTexture(RenderTargetDesc, TEXT("RDGRenderTarget"));

	//Setup Parameters
	FSimpleUniformStructParameters StructParameters;
	StructParameters.Color1 = FVector4f(1.0);
	StructParameters.Color2 = FVector4f(1.0);
	StructParameters.Color3 = FVector4f(1.0);
	StructParameters.Color4 = FVector4f(1.0);
	StructParameters.ColorIndex = uint32(0);

	FDIYShaderPS::FParameters* Parameters = GraphBuilder.AllocParameters<FDIYShaderPS::FParameters>();
	Parameters->TextureVal = InTextureRHI;
	Parameters->TextureSampler = TStaticSamplerState<SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters->SimpleColor = InColor;
	Parameters->SimpleUniformStruct = TUniformBufferRef<FSimpleUniformStructParameters>::CreateUniformBufferImmediate(
		StructParameters, UniformBuffer_SingleFrame);
	Parameters->RenderTargets[0] = FRenderTargetBinding(RDGRenderTarget, ERenderTargetLoadAction::ENoAction);

	// Get shaders.
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FDIYShaderVS> VertexShader(GlobalShaderMap);
	TShaderMapRef<FDIYShaderPS> PixelShader(GlobalShaderMap);


	GraphBuilder.AddPass(
		RDG_EVENT_NAME("DIYShader Pass"),
		Parameters,
		ERDGPassFlags::Raster,
		[Parameters, VertexShader, PixelShader, InColor](FRHICommandListImmediate& RHICmdList)
		{
			// Set RT & Viewport
			FRHITexture2D* RT = Parameters->RenderTargets[0].GetTexture()->GetRHI()->GetTexture2D();
			RHICmdList.SetViewport(0, 0, 0.0f, RT->GetSizeX(), RT->GetSizeY(), 1.0f);

			// Set the graphic pipeline state.
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.SetStreamSource(0, GScreenRectangleVertexBuffer.VertexBufferRHI, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
		});
	GraphBuilder.QueueTextureExtraction(RDGRenderTarget, &PooledRenderTarget);
	GraphBuilder.Execute();
	//Copy Result To RenderTarget Asset
	RHICmdList.CopyTexture(PooledRenderTarget->GetRHI(), RenderTargetRHI->GetTexture2D(), FRHICopyTextureInfo());
}



void UDIYShaderBlueprintFunctionLib::DrawColorBoard(
	const UObject* WorldContextObject,
	UTextureRenderTarget2D* OutputRenderTarget,
	FSimpleShaderParameter Parameter,
	UTexture2D* InTexture,
	FLinearColor InColor)
{
	check(IsInGameThread());

	if (!OutputRenderTarget)
	{
		FMessageLog("Blueprint").Warning(
			LOCTEXT("UDIYShaderBlueprintLibrary::DrawColorBoard",
			        "DrawUVDisplacementToRenderTarget: Output render target is required."));
		return;
	}

	FTexture2DRHIRef RenderTargetRHI = OutputRenderTarget->GameThread_GetRenderTargetResource()->
	                                                       GetRenderTargetTexture();
	FTexture2DRHIRef InTextureRHI = InTexture->GetResource()->TextureRHI->GetTexture2D();
	ERHIFeatureLevel::Type FeatureLevel = WorldContextObject->GetWorld()->Scene->GetFeatureLevel();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)
	(
		[RenderTargetRHI, Parameter, InTextureRHI, InColor, FeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
			RDGDraw(
				RHICmdList,
				RenderTargetRHI,
				Parameter,
				InTextureRHI,
				InColor,
				FeatureLevel
			);
		}
	);
}

	
static void RDGCSRWTextureTest(
	FRHICommandListImmediate& RHICmdList,
	FTexture2DRHIRef InTextureRHI,
	ERHIFeatureLevel::Type FeatureLevel)
{
	check(IsInRenderingThread());

	FRDGBuilder GraphBuilder(RHICmdList);

	const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	const TShaderMapRef<FDIYShaderCS> ComputeShader(GlobalShaderMap);
	
	
	FRDGTextureRef RenderTargetRDG = RegisterExternalTexture(GraphBuilder, InTextureRHI, TEXT("InRWTexture"));
	FRDGTextureDesc TmpRWTextureDesc = FRDGTextureDesc::Create2D(
		InTextureRHI->GetSizeX(),
		InTextureRHI->GetFormat(),
		FClearValueBinding::Black,
		TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV);
	FRDGTextureRef TmpRWTextureRDG = GraphBuilder.CreateTexture(TmpRWTextureDesc, TEXT("TmpRTTexture"));
	
	FDIYShaderCS::FParameters* Parameters = GraphBuilder.AllocParameters<FDIYShaderCS::FParameters>();
	Parameters->InRenderTarget = RenderTargetRDG;
	Parameters->InRTSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters->OutUAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(TmpRWTextureRDG, 0));
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("RWTextureTestPass"),
		ComputeShader,
		Parameters,
		FIntVector(InTextureRHI->GetSizeX(), InTextureRHI->GetSizeY(), 1)
	);

	AddCopyTexturePass(GraphBuilder, TmpRWTextureRDG, RenderTargetRDG);

	GraphBuilder.Execute();
}
	
void UDIYShaderBlueprintFunctionLib::DrawColorCS(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget)
{
	check(IsInGameThread());
	if(!OutputRenderTarget)
	{
		FMessageLog("Blueprint").Warning(
	LOCTEXT("UDIYShaderBlueprintLibrary::DrawColorCS",
			"DrawUVDisplacementToRenderTarget: Intexture is required."));
		return;
	}
	
	
	FTexture2DRHIRef InTextureRHI = OutputRenderTarget->GameThread_GetRenderTargetResource()->
														   GetRenderTargetTexture();
	ERHIFeatureLevel::Type FeatureLevel = WorldContextObject->GetWorld()->Scene->GetFeatureLevel();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)
	(
		[InTextureRHI, FeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
			RDGCSRWTextureTest(
				RHICmdList,
				InTextureRHI,
				FeatureLevel
			);
		}
	);
}


#undef LOCTEXT_NAMESPACE
