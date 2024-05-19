// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldRenderer.h"

#include "EngineModule.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"
#include "GlobalShader.h"
#include "SceneView.h"
#include "RHIUtilities.h"
#include "Runtime/Renderer/Private/SceneRendering.h"
#include "SceneRelativeViewMatrices.h"
#include "SettingsManager.h"


// BEGIN_SHADER_PARAMETER_STRUCT(FWindFieldParticleParameters, )
// 	SHADER_PARAMETER_RDG_BUFFER_UAV(RWTexture2D<float4>, PrevParticleTexture)
// 	SHADER_PARAMETER_RDG_BUFFER_UAV(RWTexture2D<float4>, CurrParticleTexture)
// END_SHADER_PARAMETER_STRUCT()

/* Particle Compute Shader Class */
// class FDIYDrawColorCS : public FGlobalShader
// {
// 	DECLARE_GLOBAL_SHADER(FDIYDrawColorCS)
// 	SHADER_USE_PARAMETER_STRUCT(FDIYDrawColorCS, FGlobalShader)
//
// 	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
// 	{
// 		return true;
// 	}
// 	
// 	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
// 		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, WindFieldTexture)
// 		SHADER_PARAMETER_SAMPLER(SamplerState, WindFieldTextureSampler)
// 		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, CurrParticleMap)
// 		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ParticleSpeedMap)
// 		SHADER_PARAMETER_SAMPLER(SamplerState, ParticleMapSampler)
// 		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutCurrPositionTexture)
// 		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutParticleSpeedTexture)
// 		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutTrailPositionTexture)
// 		SHADER_PARAMETER(float, RandomCoefficient)
// 		SHADER_PARAMETER_STRUCT_INCLUDE(FlowFieldSettingsParameters, FlowFieldSettings)
// 	END_SHADER_PARAMETER_STRUCT()
// };
//
// IMPLEMENT_SHADER_TYPE(, FDIYDrawColorCS, TEXT("/Plugin/DIYShader/Private/ComputeParticlePosCS.usf"), TEXT("MainCS"), SF_Compute);\



BEGIN_SHADER_PARAMETER_STRUCT(FComputeParticleParameters, )
// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticlePosition)
// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticleSpeed)
SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticlePosition)
SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticleSpeed)
SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWTrailPosition)
SHADER_PARAMETER(uint32, TrailIndex)
SHADER_PARAMETER(float, RandomCoefficient)
SHADER_PARAMETER_STRUCT_INCLUDE(FlowFieldSettingsParameters, FlowFieldSettings)
SHADER_PARAMETER(FIntVector, ThreadGroupCount)
END_SHADER_PARAMETER_STRUCT()


/* Compute Using Buffers */
class FComputeParticlePosCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FComputeParticlePosCS)
	SHADER_USE_PARAMETER_STRUCT(FComputeParticlePosCS, FGlobalShader)

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, FlowFieldTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, FlowFieldTextureSampler)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticlePosition)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticleSpeed)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticlePosition)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticleSpeed)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWTrailPosition)
		SHADER_PARAMETER(uint32, TrailIndex)
		SHADER_PARAMETER(float, RandomCoefficient)
		SHADER_PARAMETER_STRUCT_INCLUDE(FlowFieldSettingsParameters, FlowFieldSettings)
		SHADER_PARAMETER(FIntVector, ThreadGroupCount)
	END_SHADER_PARAMETER_STRUCT()
};

class FComputeParticlePos3DCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FComputeParticlePos3DCS)
	SHADER_USE_PARAMETER_STRUCT(FComputeParticlePos3DCS, FGlobalShader)

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE(Texture3D, FlowFieldTexture3D)
		SHADER_PARAMETER_SAMPLER(SamplerState, FlowFieldTexture3DSampler)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticlePosition)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticleSpeed)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticlePosition)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWParticleSpeed)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, RWTrailPosition)
		SHADER_PARAMETER(uint32, TrailIndex)
		SHADER_PARAMETER(float, RandomCoefficient)
		SHADER_PARAMETER_STRUCT_INCLUDE(FlowFieldSettingsParameters, FlowFieldSettings)
		SHADER_PARAMETER(FIntVector, ThreadGroupCount)
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_SHADER_TYPE(, FComputeParticlePosCS, TEXT("/Plugin/VisOnEarth/Private/ComputeParticlePos.usf"),
                        TEXT("MainCS"), SF_Compute);

IMPLEMENT_SHADER_TYPE(, FComputeParticlePos3DCS, TEXT("/Plugin/VisOnEarth/Private/ComputeParticlePos.usf"),
                        TEXT("MainCS_3D"), SF_Compute);

/* Line Drawing Graphics Shader Class */
// Parent Class: Defining Shared Shader Parameters
class FDrawLineFromPositionMapShader : public FGlobalShader 
{
	// CANNOT DECLARE SHADER BASE CLASS AS GLOBAL SHADER 
	// DECLARE_GLOBAL_SHADER(FDrawLineFromPositionMapShader)
public:
	SHADER_USE_PARAMETER_STRUCT(FDrawLineFromPositionMapShader, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
		SHADER_PARAMETER(FMatrix44f, WorldToClipMatrix)
		SHADER_PARAMETER(FMatrix44f, ECEFToLocalMatrix)
#if FLOWFIELD_USE_COMPUTE
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticlePosition)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, ParticleSpeed)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, TrailPosition)
#else
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, CurrParticleMap)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ParticleSpeedMap)
		SHADER_PARAMETER_RDG_TEXTURE_ARRAY(Texture2D, TrailParticleTextures, [FFlowFieldRenderer::MaxTrailLength])
		SHADER_PARAMETER_SAMPLER(SamplerState, ParticleMapSampler)
		SHADER_PARAMETER(FUint32Vector2, TexSize)
#endif
		SHADER_PARAMETER(uint32, TrailIndex)
		SHADER_PARAMETER_STRUCT_INCLUDE(FlowFieldSettingsParameters, FlowFieldSettings)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// return EnumHasAllFlags(Parameters.Flags, EShaderPermutationFlags::HasEditorOnlyData) && IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
		return true;
	}
};


class FDrawLineFromPositionMapVS : public FDrawLineFromPositionMapShader
{
	DECLARE_SHADER_TYPE(FDrawLineFromPositionMapVS, Global);

	FDrawLineFromPositionMapVS() {}
	FDrawLineFromPositionMapVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDrawLineFromPositionMapShader(Initializer)
	{

	}
};

class FDrawLineFromPositionMapPS : public FDrawLineFromPositionMapShader
{
	DECLARE_GLOBAL_SHADER(FDrawLineFromPositionMapPS);
	
	FDrawLineFromPositionMapPS() {}
	FDrawLineFromPositionMapPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDrawLineFromPositionMapShader(Initializer)
	{

	}
};



#if FLOWFIELD_USE_COMPUTE
IMPLEMENT_SHADER_TYPE(, FDrawLineFromPositionMapVS, TEXT("/Plugin/VisOnEarth/Private/DrawParticle.usf"), TEXT("MainVS"), SF_Vertex);
#else
IMPLEMENT_SHADER_TYPE(, FDrawLineFromPositionMapVS, TEXT("/Plugin/VisOnEarth/Private/DrawLineFromPositionMap.usf"), TEXT("MainVS"), SF_Vertex);
#endif
IMPLEMENT_SHADER_TYPE(, FDrawLineFromPositionMapPS, TEXT("/Plugin/VisOnEarth/Private/DrawLineFromPositionMap.usf"), TEXT("MainPS"), SF_Pixel);


class FDrawLineTest : public FGlobalShader
{
public:
	SHADER_USE_PARAMETER_STRUCT(FDrawLineTest, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
		SHADER_PARAMETER(FMatrix44f, WorldToClipMatrix)
		SHADER_PARAMETER(FMatrix44f, ECEFToLocalMatrix)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// return EnumHasAllFlags(Parameters.Flags, EShaderPermutationFlags::HasEditorOnlyData) && IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
		return true;
	}
};

class FDrawLineTestVS : public FDrawLineTest
{
	DECLARE_GLOBAL_SHADER(FDrawLineTestVS);
	
public:
	FDrawLineTestVS() {}
	FDrawLineTestVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDrawLineTest(Initializer)
	{

	}
	
};

class FDrawLineTestPS : public FDrawLineTest
{
	DECLARE_GLOBAL_SHADER(FDrawLineTestPS);

public:
	FDrawLineTestPS() {}
	FDrawLineTestPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FDrawLineTest(Initializer)
	{

	}
};

IMPLEMENT_GLOBAL_SHADER(FDrawLineTestVS, "/Plugin/VisOnEarth/Private/DrawLineTest.usf", "MainVS", SF_Vertex)
IMPLEMENT_GLOBAL_SHADER(FDrawLineTestPS, "/Plugin/VisOnEarth/Private/DrawLineTest.usf", "MainPS", SF_Pixel)













/* FFlowFieldRenderer Initialize Functions */
FFlowFieldRenderer::FFlowFieldRenderer()
{

}

FFlowFieldRenderer::~FFlowFieldRenderer()
{
}

void FFlowFieldRenderer::RegisterRendererDelegate()
{
	check(IsInRenderingThread())

	if(RenderHandle.IsValid()) return ;

	RenderHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateRaw(this, &FFlowFieldRenderer::Render));

	UE_LOG(LogTemp, Warning, TEXT("FFlowFieldRenderer is Registered!"));
}

void FFlowFieldRenderer::UnregisterRendererDelegate()
{
	// check(IsInRenderingThread())

	if(!RenderHandle.IsValid()) return;

	GetRendererModule().RemovePostOpaqueRenderDelegate(RenderHandle);
	RenderHandle.Reset();

	UE_LOG(LogTemp, Warning, TEXT("FFlowFieldRenderer is Destroyed!"));
}


TGlobalResource<FLineVertexForInstanceDrawDeclaration> GLineVertexForInstanceDrawDeclaration;
TGlobalResource<FTriVertexForInstanceDrawDeclaration> GTriVertexForInstanceDrawDeclaration;

void FFlowFieldRenderer::AddDrawLinePass(FPostOpaqueRenderParameters& InParameters) {
	//Draw Pass
	//InParameters.ColorTexture
	//FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, PrimaryViewRect);
	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	const FViewInfo& View = *InParameters.View;
	// To-Do: Get FeatureLevel From Scene Settings ?
	const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;
	const FRelativeViewMatrices RelativeMatrices = FRelativeViewMatrices::Create(View.ViewMatrices);
	
	RDG_EVENT_SCOPE(GraphBuilder, "AddDrawLinePass");
	
	
	
	auto* PassParameters = GraphBuilder.AllocParameters<FDrawLineFromPositionMapShader::FParameters>();
	PassParameters->View = InParameters.View->ViewUniformBuffer;
	PassParameters->SceneTextures = InParameters.SceneTexturesUniformParams;
	PassParameters->WorldToClipMatrix = FMatrix44f(InParameters.View->ViewMatrices.GetViewProjectionMatrix());
	PassParameters->ECEFToLocalMatrix = FMatrix44f(GeoReference->ComputeEarthCenteredEarthFixedToUnrealTransformation());
	// PassParameters->ECEFToLocalMatrix = ConvertGLMMatrixToFMatrix(GeoReference->GetCoordinateSystem().getEcefToLocalTransformation());
	
#if FLOWFIELD_USE_COMPUTE
	// FRDGBufferRef RWPos = GraphBuilder.RegisterExternalBuffer(Resources.ParticlePosition.GetPooledBuffer());
	// FRDGBufferRef RWSpeed = GraphBuilder.RegisterExternalBuffer(Resources.ParticleSpeed.GetPooledBuffer());
	// FRDGBufferRef RWTrail = GraphBuilder.RegisterExternalBuffer(Resources.TrailPosition.GetPooledBuffer());
	// PassParameters->ParticlePosition = GraphBuilder.CreateSRV(RWPos, PF_A32B32G32R32F);
	// PassParameters->ParticleSpeed = GraphBuilder.CreateSRV(RWSpeed,PF_A32B32G32R32F);
	// PassParameters->TrailPosition = GraphBuilder.CreateSRV(RWTrail, PF_A32B32G32R32F);
	PassParameters->ParticlePosition = Resources.ParticlePosition.GetOrCreateSRV(GraphBuilder);
	PassParameters->ParticleSpeed =  Resources.ParticleSpeed.GetOrCreateSRV(GraphBuilder);
	PassParameters->TrailPosition =  Resources.TrailPosition.GetOrCreateSRV(GraphBuilder);


#else
	
	FRDGTextureRef CurrParticleTextureExternal = RegisterExternalTexture(GraphBuilder, CurrParticleTextureRHI, TEXT("InCurrParticleTexture"));
	FRDGTextureRef ParticleSpeedTextureExternal = RegisterExternalTexture(GraphBuilder, ParticleSpeedTextureRHI, TEXT("InParticleSpeedTexture"));
	PassParameters->CurrParticleMap = CurrParticleTextureExternal;
	PassParameters->ParticleSpeedMap = ParticleSpeedTextureExternal;
	for(uint32 i=0;i<MaxTrailLength;i++)
	{
		PassParameters->TrailParticleTextures[i] = RegisterExternalTexture(GraphBuilder, TrailParticleTexturesRHI[i], *FString::Printf(TEXT("InTrailParticleTexture%d"), i) );
	}
	PassParameters->ParticleMapSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	PassParameters->TexSize = TexSize;
#endif
	

	PassParameters->TrailIndex = FrameCounter;
	
	PassParameters->FlowFieldSettings = GetFlowFieldSettingsParameters(Settings);

	PassParameters->RenderTargets[0] = FRenderTargetBinding(InParameters.ColorTexture, ERenderTargetLoadAction::ELoad);
	PassParameters->RenderTargets.DepthStencil = FDepthStencilBinding(InParameters.DepthTexture, ERenderTargetLoadAction::ELoad, ERenderTargetLoadAction::ELoad, FExclusiveDepthStencil::DepthWrite_StencilWrite);

	
	const FIntRect ViewportRect = InParameters.ViewportRect;
	const FIntPoint TextureExtent = InParameters.ColorTexture->Desc.Extent;


	
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("DrawLinePass"),
		PassParameters,
		ERDGPassFlags::Raster,
		[ViewportRect, PassParameters, FeatureLevel, this] (FRHICommandList& RHICmdList)
		{
			RHICmdList.SetViewport(0, 0, 0.0f, ViewportRect.Width(), ViewportRect.Height(), 1.0f);
			const TShaderMapRef<FDrawLineFromPositionMapVS> VertexShader(GetGlobalShaderMap(FeatureLevel));
			const TShaderMapRef<FDrawLineFromPositionMapPS> PixelShader(GetGlobalShaderMap(FeatureLevel));

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None, true>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<true, CF_Greater>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_LineList;
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GLineVertexForInstanceDrawDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *PassParameters);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), *PassParameters);
			
			RHICmdList.SetStreamSource(0, Resources.VertexBuffer, 0);
			int ArrowInstanceCount = Settings.DrawArrow ? 2 : 0; // left + right = 2 instance
			// Only Available When Using Vulkan RHI Context
#if FLOWFIELD_USE_VARIABLE_LINEWIDTH
			RHICmdList.DrawLineWithWidth(0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + ArrowInstanceCount), Settings.LineWidth);
#else
			RHICmdList.DrawPrimitive(0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + 2));
			#endif
			
			// RHICmdList.DrawIndexedPrimitive(Resources.IndexBuffer, 0, 0, 2, 0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + 2));
		});
}
// Ugly Implementation: might cause many threads waste at cases
bool TryDivideThreadGroups(int Num, FIntVector3& ThreadGroupCount)
{
	float UnAllocatedWorks = StaticCast<float>(Num);
	if(UnAllocatedWorks >= GRHIMaxDispatchThreadGroupsPerDimension.X)
	{
		ThreadGroupCount.X = GRHIMaxDispatchThreadGroupsPerDimension.X;
		UnAllocatedWorks  = UnAllocatedWorks * 1.0f /GRHIMaxDispatchThreadGroupsPerDimension.X;
	}else
	{
		ThreadGroupCount.X = UnAllocatedWorks;
		return true;
	}

	if(UnAllocatedWorks >= GRHIMaxDispatchThreadGroupsPerDimension.Y)
	{
		ThreadGroupCount.Y = GRHIMaxDispatchThreadGroupsPerDimension.Y;
		UnAllocatedWorks  = UnAllocatedWorks * 1.0f / GRHIMaxDispatchThreadGroupsPerDimension.Y;
	}else
	{
		ThreadGroupCount.Y = FMath::RoundToInt(UnAllocatedWorks);
		return true;
	}

	if(UnAllocatedWorks >= GRHIMaxDispatchThreadGroupsPerDimension.Z)
	{
		ThreadGroupCount.Z = GRHIMaxDispatchThreadGroupsPerDimension.Z;
		int AllocatedWorks = GRHIMaxDispatchThreadGroupsPerDimension.X*GRHIMaxDispatchThreadGroupsPerDimension.Y*GRHIMaxDispatchThreadGroupsPerDimension.Z;
		UE_LOG(LogTemp,Error, TEXT("Cannot Dispatch All Compute Works To ThreadGroups, %d Works are Allocated, %d is Not!"),AllocatedWorks, Num-AllocatedWorks);
		UE_LOG(LogTemp,Error, TEXT("GPU's Max DispatchThreadGroupPerDimensions are %d, %d, %d"),GRHIMaxDispatchThreadGroupsPerDimension.X, GRHIMaxDispatchThreadGroupsPerDimension.Y, GRHIMaxDispatchThreadGroupsPerDimension.Z);
		return false;
	}else
	{
		ThreadGroupCount.Z = FMath::RoundToInt(UnAllocatedWorks);
		return true;
	}

	return false;
	
}
#if FLOWFIELD_USE_COMPUTE
void FFlowFieldRenderer::AddComputeParticleCSPass(FPostOpaqueRenderParameters& InParameters) {
	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	const FViewInfo& View = *InParameters.View;

	// To-Do: Get FeatureLevel From Scene Settings ?
	const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;

	const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);

	if(Resources.DataType == FieldDataDimension::D2)
	{
		const TShaderMapRef<FComputeParticlePosCS> ComputeShader(GlobalShaderMap);
		FComputeParticlePosCS::FParameters* PassParams = GraphBuilder.AllocParameters<FComputeParticlePosCS::FParameters>();
		PassParams->FlowFieldTexture = RegisterExternalTexture(GraphBuilder, Resources.FlowFieldTexture, TEXT("FlowFieldTexture"));
		PassParams->FlowFieldTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();


		// FRDGBufferRef RWPos = GraphBuilder.RegisterExternalBuffer(Resources.ParticlePosition.GetPooledBuffer());
		// FRDGBufferRef RWSpeed = GraphBuilder.RegisterExternalBuffer(Resources.ParticleSpeed.GetPooledBuffer());
		// FRDGBufferRef RWTrail = GraphBuilder.RegisterExternalBuffer(Resources.TrailPosition.GetPooledBuffer());
		// PassParams->RWParticlePosition = GraphBuilder.CreateUAV(RWPos, PF_A32B32G32R32F);
		// PassParams->RWParticleSpeed = GraphBuilder.CreateUAV(RWSpeed, PF_A32B32G32R32F);
		// PassParams->RWTrailPosition = GraphBuilder.CreateUAV(RWTrail, PF_A32B32G32R32F);
		PassParams->RWParticlePosition = Resources.ParticlePosition.GetOrCreateUAV(GraphBuilder);
		PassParams->RWParticleSpeed = Resources.ParticleSpeed.GetOrCreateUAV(GraphBuilder);
		PassParams->RWTrailPosition = Resources.TrailPosition.GetOrCreateUAV(GraphBuilder);
	
		PassParams->FlowFieldSettings = GetFlowFieldSettingsParameters(Settings);
		PassParams->RandomCoefficient = FMath::RandRange(0.0, 1.0);
		PassParams->TrailIndex = FrameCounter;
	
		// Thread Group: x * y * z, numthreads: 1,1,1
		FIntVector ThreadGroupCount (1,1,1);
		if(!TryDivideThreadGroups(Settings.ParticlesSum, ThreadGroupCount)) return ;

		PassParams->ThreadGroupCount = ThreadGroupCount;

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("ComputeParticlePositionPass"),
			ComputeShader,
			PassParams,
			ThreadGroupCount
		);
	}else if(Resources.DataType == FieldDataDimension::D3)
	{
		const TShaderMapRef<FComputeParticlePos3DCS> ComputeShader(GlobalShaderMap);
		FComputeParticlePos3DCS::FParameters* PassParams = GraphBuilder.AllocParameters<FComputeParticlePos3DCS::FParameters>();
		PassParams->FlowFieldTexture3D = RegisterExternalTexture(GraphBuilder, Resources.FlowField3DTexture, TEXT("FlowFieldTexture"));
		PassParams->FlowFieldTexture3DSampler = TStaticSamplerState<SF_Trilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();

		// FRDGBufferRef RWPos = GraphBuilder.RegisterExternalBuffer(Resources.ParticlePosition.GetPooledBuffer());
		// FRDGBufferRef RWSpeed = GraphBuilder.RegisterExternalBuffer(Resources.ParticleSpeed.GetPooledBuffer());
		// FRDGBufferRef RWTrail = GraphBuilder.RegisterExternalBuffer(Resources.TrailPosition.GetPooledBuffer());
		// PassParams->RWParticlePosition = GraphBuilder.CreateUAV(RWPos, PF_A32B32G32R32F);
		// PassParams->RWParticleSpeed = GraphBuilder.CreateUAV(RWSpeed, PF_A32B32G32R32F);
		// PassParams->RWTrailPosition = GraphBuilder.CreateUAV(RWTrail, PF_A32B32G32R32F);
		PassParams->RWParticlePosition = Resources.ParticlePosition.GetOrCreateUAV(GraphBuilder);
		PassParams->RWParticleSpeed = Resources.ParticleSpeed.GetOrCreateUAV(GraphBuilder);
		PassParams->RWTrailPosition = Resources.TrailPosition.GetOrCreateUAV(GraphBuilder);
	
		PassParams->FlowFieldSettings = GetFlowFieldSettingsParameters(Settings);
		PassParams->RandomCoefficient = FMath::RandRange(0.0, 1.0);
		PassParams->TrailIndex = FrameCounter;
	
		// Thread Group: x * y * z, numthreads: 1,1,1
		FIntVector ThreadGroupCount (1,1,1);
		if(!TryDivideThreadGroups(Settings.ParticlesSum, ThreadGroupCount)) return ;

		PassParams->ThreadGroupCount = ThreadGroupCount;

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("ComputeParticlePositionPass"),
			ComputeShader,
			PassParams,
			ThreadGroupCount
		);
	}




	// AddCopyBufferPass()
}
#else
void FFlowFieldRenderer::AddComputeParticlePSPass(FPostOpaqueRenderParameters& InParameters) {
	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	const FViewInfo& View = *InParameters.View;

	// To-Do: Get FeatureLevel From Scene Settings ?
	const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;

	const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	const TShaderMapRef<FDIYDrawColorCS> ComputeShader(GlobalShaderMap);

	RDG_EVENT_SCOPE(GraphBuilder, "AddComputeParticlePass");
	

	FRDGTextureRef CurrParticleTextureExternal = RegisterExternalTexture(GraphBuilder, CurrParticleTextureRHI, TEXT("InCurrParticleTexture"));
	FRDGTextureRef ParticleSpeedTextureExternal = RegisterExternalTexture(GraphBuilder, ParticleSpeedTextureRHI, TEXT("InParticleSpeedTexture"));

	FRDGTextureRef TrailParticleTextureExternal = RegisterExternalTexture(GraphBuilder, TrailParticleTexturesRHI[FrameCounter], TEXT("InTrailParticleTexture"));
	
	
	FRDGTextureDesc VisTexDesc = FRDGTextureDesc::Create2D(
		FIntPoint(CurrParticleTextureRHI->GetSizeX(), CurrParticleTextureRHI->GetSizeY()),
		CurrParticleTextureRHI->GetFormat(),
		FClearValueBinding::None,
		TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV
	);

	FRDGTextureRef OutTrailParticleTextureRDG = GraphBuilder.CreateTexture(
		VisTexDesc, TEXT("OutTrailParticleTexture"));
	
	FRDGTextureRef OutCurrPositionTextureRDG = GraphBuilder.CreateTexture(
		VisTexDesc, TEXT("OutCurrPositionTexture"));

	FRDGTextureRef OutParticleSpeedTextureRDG = GraphBuilder.CreateTexture(
		VisTexDesc, TEXT("OutParticleSpeedTexture"));

	// Register OutSide WindField Texture
	FRDGTextureRef WindFieldRDGTexture = RegisterExternalTexture(GraphBuilder, FRHIWindFieldTexture, TEXT("WindFieldTexture"));

	// Compute Shader Parameters and Textures
	FDIYDrawColorCS::FParameters* CSParams = GraphBuilder.AllocParameters<FDIYDrawColorCS::FParameters>();
	// CSParams->WindFieldTextureSRV = WindFieldRDGTexture;
	CSParams->WindFieldTexture = WindFieldRDGTexture;
	CSParams->WindFieldTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	CSParams->CurrParticleMap = CurrParticleTextureExternal;
	CSParams->ParticleSpeedMap = ParticleSpeedTextureExternal;
	CSParams->ParticleMapSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	CSParams->OutCurrPositionTexture = GraphBuilder.CreateUAV(OutCurrPositionTextureRDG);
	CSParams->OutParticleSpeedTexture = GraphBuilder.CreateUAV(OutParticleSpeedTextureRDG);
	CSParams->OutTrailPositionTexture = GraphBuilder.CreateUAV(OutTrailParticleTextureRDG);
	
	CSParams->FlowFieldSettings = GetFlowFieldSettingsParameters(FlowFieldSettings);
	CSParams->RandomCoefficient = FMath::RandRange(0.0, 1.0);

	// Thread Group: width * height, numthreads: 1,1,1
	FIntVector ThreadGroupCount (1024, 1024, 1);
	// Add Compute Pass, Render Radiation Data to VolumeTexture
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("ComputeParticlePositionPass"),
		ComputeShader,
		CSParams,
		ThreadGroupCount
	);


	AddCopyTexturePass(GraphBuilder, OutCurrPositionTextureRDG, CurrParticleTextureExternal);
	AddCopyTexturePass(GraphBuilder, OutParticleSpeedTextureRDG, ParticleSpeedTextureExternal);
	AddCopyTexturePass(GraphBuilder, OutTrailParticleTextureRDG, TrailParticleTextureExternal);
	
}
#endif

void FFlowFieldRenderer::AddTestPass(FPostOpaqueRenderParameters& InParameters)
{

	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	const FViewInfo& View = *InParameters.View;
	// To-Do: Get FeatureLevel From Scene Settings ?
	const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;
	const FRelativeViewMatrices RelativeMatrices = FRelativeViewMatrices::Create(View.ViewMatrices);
	
	RDG_EVENT_SCOPE(GraphBuilder, "AddTestPass");
	
	
	auto* PassParameters = GraphBuilder.AllocParameters<FDrawLineTest::FParameters>();
	PassParameters->View = InParameters.View->ViewUniformBuffer;
	PassParameters->SceneTextures = InParameters.SceneTexturesUniformParams;
	PassParameters->WorldToClipMatrix = FMatrix44f(InParameters.View->ViewMatrices.GetViewProjectionMatrix());
	PassParameters->ECEFToLocalMatrix = FMatrix44f(GeoReference->ComputeEarthCenteredEarthFixedToUnrealTransformation());
	
	PassParameters->RenderTargets[0] = FRenderTargetBinding(InParameters.ColorTexture, ERenderTargetLoadAction::ELoad);
	PassParameters->RenderTargets.DepthStencil = FDepthStencilBinding(InParameters.DepthTexture, ERenderTargetLoadAction::ELoad, ERenderTargetLoadAction::ELoad, FExclusiveDepthStencil::DepthWrite_StencilWrite);

	
	const FIntRect ViewportRect = InParameters.ViewportRect;
	const FIntPoint TextureExtent = InParameters.ColorTexture->Desc.Extent;


	
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("DrawTestPass"),
		PassParameters,
		ERDGPassFlags::Raster,
		[ViewportRect, PassParameters, FeatureLevel, this] (FRHICommandList& RHICmdList)
		{
			RHICmdList.SetViewport(0, 0, 0.0f, ViewportRect.Width(), ViewportRect.Height(), 1.0f);
			const TShaderMapRef<FDrawLineTestVS> VertexShader(GetGlobalShaderMap(FeatureLevel));
			const TShaderMapRef<FDrawLineTestPS> PixelShader(GetGlobalShaderMap(FeatureLevel));

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<true, CF_Greater>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			// GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_LineList;
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GTriVertexForInstanceDrawDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *PassParameters);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), *PassParameters);

			

			RHICmdList.SetStreamSource(0, Resources.TmpVertexBuffer, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
			// RHICmdList.DrawLineWithWidth(0, 2, 1, 5.0);
			// Only Available When Using Vulkan RHI Context
// #if FLOWFIELD_USE_VARIABLE_LINEWIDTH
// 			RHICmdList.DrawLineWithWidth(0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + 2), Settings.LineWidth);
// #else
// 			RHICmdList.DrawPrimitive(0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + 2));
// 			#endif
			
			// RHICmdList.DrawIndexedPrimitive(Resources.IndexBuffer, 0, 0, 2, 0, 1, Settings.ParticlesSum * (1 + Settings.TrailLength + 2));
		});
}


void FFlowFieldRenderer::Render(FPostOpaqueRenderParameters& InParameters)
{

	if(!Resources.IsValid())
	{
		if(!isInitializing)
		{
			Resources.InitializeFromAnyThread(Settings);
			Resources.InitializePooledRWBuffers(*InParameters.GraphBuilder, Settings);
			isInitializing = true;
		}
		return ;
	}
	
	isInitializing = false;

	if(GeoReference == nullptr) return;

	if(Settings != SettingsFromGame){
		if(Settings.JsonDataFile!= SettingsFromGame.JsonDataFile)
		{
			Resources.InitializeFlowFieldTexture(SettingsFromGame);
		}
		if(Settings.ParticlesSum!=SettingsFromGame.ParticlesSum || Settings.TrailLength!=SettingsFromGame.TrailLength)
		{
			Resources.InitializePooledRWBuffers(*InParameters.GraphBuilder, SettingsFromGame);	
		}
		Settings = SettingsFromGame;
		return ;
	}

	// Use FrameCounter to Decide Trail Index
	FrameCounter = (FrameCounter+1) % Settings.TrailLength;

	
	// Compute Particles Pass, Write Result to Buffer / Map
#if FLOWFIELD_USE_COMPUTE
	AddComputeParticleCSPass(InParameters);
#else
	AddComputeParticlePSPass(InParameters);
#endif
	
	// DrawLines Pass:
	// 1. Transoform Particle Position to Cesium Earth
	// 2. Generate Color For Particles
	AddDrawLinePass(InParameters);

	// AddTestPass(InParameters);
	
	// Clear All Refs Created By this Frame's GraphBuilder
	Resources.EndPooledRWBuffersGraphUsage();
}

void FFlowFieldRenderer::UpdateGeoReference(ACesiumGeoreference* NewGeoReference)
{
	ENQUEUE_RENDER_COMMAND(UpdateGeoReference)(
	[this, NewGeoReference](FRHICommandListImmediate& RHICmdList)
	{
		GeoReference = NewGeoReference;
	});
	
}

FFlowFieldSettings FFlowFieldRenderer::GetSetttings()
{
	return this->Settings;
}

void FFlowFieldRenderer::UpdateSettingsToGame()
{
	AsyncTask(ENamedThreads::GameThread, [NewSettings = this->Settings]()
	{
		
	});
}

void FFlowFieldRenderer::UpdateSettingsFromGame(FFlowFieldSettings NewSettingsFromGame)
{
	ENQUEUE_RENDER_COMMAND(UpdateGeoReference)(
	[this, NewSettingsFromGame](FRHICommandListImmediate& RHICmdList)
	{
		SettingsFromGame = NewSettingsFromGame;
	});
}


bool FFlowFieldRenderer::isMultiFrameResourceInitialized()
{
	return true;
		// Must Not Use CreateStructuredDesc !!!
	// if Use CreateStructuredDesc rather than CreateBufferDesc, vkWriteDescriptorSet will expect 'TEXEL BUFFER', while trying to write with TYPE 'STORAGE BUFFER'
	// When Using Must CreateBufferDesc, buffer will be treated as typed Buffer and thus must Specify PF Format when creating UAV: graphbuilder.createUAV(rdgbuffer, PF_XXXX); !!!
	// if(!PBPos.IsValid())
	// {
	// 	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	// 	FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), FlowFieldSettings.ParticlesSum);
	// 	// BufferDesc.Usage |= UsageFlags;
	// 	FRDGBufferRef BufferRef = GraphBuilder.CreateBuffer(BufferDesc, TEXT("ParticlePositionBuffer"));
	// 	PBPos = GraphBuilder.ConvertToExternalBuffer(BufferRef);
	// 	TArray<FVector4f>InitialArr;
	// 	InitialArr.SetNumUninitialized(FlowFieldSettings.ParticlesSum);
	// 	for(int i=0;i<FlowFieldSettings.ParticlesSum;i++)
	// 	{
	// 		InitialArr[i] = FVector4f(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 0.0f, 1.0f);
	// 	}
	// 	BufferRef = GraphBuilder.RegisterExternalBuffer(PBPos);
	// 	InParameters.GraphBuilder->QueueBufferUpload(BufferRef, InitialArr.GetData(), sizeof(FVector4f)*FlowFieldSettings.ParticlesSum);
	// 	return ;
	// }
	// if(!PBSpeed.IsValid())
	// {
	// 	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	// 	FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), FlowFieldSettings.ParticlesSum);
	// 	// BufferDesc.Usage |= UsageFlags;
	// 	FRDGBufferRef BufferRef = GraphBuilder.CreateBuffer(BufferDesc, TEXT("ParticleSpeedBuffer"));
	// 	PBSpeed = GraphBuilder.ConvertToExternalBuffer(BufferRef);
	// 	TArray<FVector4f>InitialArr;
	// 	InitialArr.SetNumUninitialized(FlowFieldSettings.ParticlesSum);
	// 	for(int i=0;i<FlowFieldSettings.ParticlesSum;i++)
	// 	{
	// 		InitialArr[i] = FVector4f(0.0f,0.0f,0.0f,0.0f);
	// 	}
	// 	BufferRef = GraphBuilder.RegisterExternalBuffer(PBSpeed);
	// 	InParameters.GraphBuilder->QueueBufferUpload(BufferRef, InitialArr.GetData(), sizeof(FVector4f)*FlowFieldSettings.ParticlesSum);
	// 	return ;
	// }
	// if(!PBTrail.IsValid())
	// {
	// 	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	// 	FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), FlowFieldSettings.ParticlesSum*FlowFieldSettings.TrailLength);
	// 	// BufferDesc.Usage |= UsageFlags;
	// 	FRDGBufferRef BufferRef = GraphBuilder.CreateBuffer(BufferDesc, TEXT("ParticleTrailBuffer"));
	// 	PBTrail = GraphBuilder.ConvertToExternalBuffer(BufferRef);
	// 	TArray<FVector4f>InitialArr;
	// 	InitialArr.SetNumUninitialized(FlowFieldSettings.ParticlesSum*FlowFieldSettings.TrailLength);
	// 	for(int i=0;i<FlowFieldSettings.ParticlesSum*FlowFieldSettings.TrailLength;i++)
	// 	{
	// 		InitialArr[i] = FVector4f(0.0f, 0.0f, 0.0f, 0.0f);
	// 	}
	// 	BufferRef = GraphBuilder.RegisterExternalBuffer(PBTrail);
	// 	InParameters.GraphBuilder->QueueBufferUpload(BufferRef, InitialArr.GetData(), sizeof(FVector4f)*FlowFieldSettings.ParticlesSum*FlowFieldSettings.TrailLength);
	// 	return ;
	// }
}

#if FLOWFIELD_USE_COMPUTE

#else
void FFlowFieldRenderer::UpdateFlowFieldTexture(UTexture2D* NewFlowFieldTexture)
{
	ENQUEUE_RENDER_COMMAND(UpdateFlowFieldTexture)(
		[this, NewFlowFieldTexture](FRHICommandListImmediate& RHICmdList)
		{
			FRHIWindFieldTexture = NewFlowFieldTexture->GetResource()->GetTexture2DRHI();
			TexSize.X = NewFlowFieldTexture->GetSizeX();
			TexSize.Y = NewFlowFieldTexture->GetSizeY();
		});
}

void FFlowFieldRenderer::UpdateParticleTexture(UTexture2D* NewCurrParticleTexture,
                                                   UTexture2D* NewParticleSpeedTexture)
{
	ENQUEUE_RENDER_COMMAND(UpdateParticleTexture)(
		[this, NewCurrParticleTexture, NewParticleSpeedTexture](FRHICommandListImmediate& RHICmdList)
		{
			CurrParticleTextureRHI = NewCurrParticleTexture->GetResource()->GetTexture2DRHI();
			ParticleSpeedTextureRHI = NewParticleSpeedTexture->GetResource()->GetTexture2DRHI();
		});
}

void FFlowFieldRenderer::UpdateTrailTexture(const TArray<UTexture2D*>& NewTrailParticleTextures,
                                                const uint32 NewTrailLength)
{
	ENQUEUE_RENDER_COMMAND(UpdateParticleTexture)(
		[this, NewTrailParticleTextures, NewTrailLength](FRHICommandListImmediate& RHICmdList)
		{
			FlowFieldSettings.TrailLength = NewTrailLength;
			for (uint32 i = 0; i < MaxTrailLength; i++)
			{
				TrailParticleTexturesRHI[i] = NewTrailParticleTextures[i]->GetResource()->GetTexture2DRHI();
			}
		});
}


void FFlowFieldRenderer::UpdateParticleSum(uint32 NewParticleSum)
{
	// NewParticleSum = FMath::Min(uint32(300*300), NewParticleSum);
	ENQUEUE_RENDER_COMMAND(CreateLineVertexBufferForParticleVisualization)(
		[this, NewParticleSum](FRHICommandListImmediate& RHICmdList)
		{
			FlowFieldSettings.ParticlesSum = NewParticleSum;
			
			// TResourceArray<FLineVertexForInstanceDraw> LineVertexArr;
			// LineVertexArr.SetNumUninitialized(2048);
			// // Must Provice Corresponding Number of Vertex
			// for(uint32 i=0;i<1024;i++)
			// {
			// 	LineVertexArr[2*i] = FLineVertexForInstanceDraw(0);
			// 	LineVertexArr[2*i+1] = FLineVertexForInstanceDraw(1);
			// }


			// Generate Vertex Wise UVTexCoord As Vertex Attribute
			// ParticleSum = 128*128;
			// TResourceArray<FLineVertexForInstanceDraw> LineVertexArr;
			// LineVertexArr.SetNumUninitialized(ParticleSum*2);
			// for(int i=0;i<128;i++)
			// {
			// 	for(int j=0;j<128;j++)
			// 	{
			// 		// LineVertexArr[2*(i*128 + j)] = FLineVertexForInstanceDraw(FVector2f((i+0.5f)/128.0f, (j+0.5f)/128.0f), 0.0f);
			// 		// LineVertexArr[2*(i*128 + j) + 1] = FLineVertexForInstanceDraw(FVector2f((i+0.5f)/128.0f, (j+0.5f)/128.0f), 1.0f);
			// 		LineVertexArr[2 * (i * 128 + j)] = FLineVertexForInstanceDraw(
			// 			FVector2f(0, 0), 0.0f);
			// 		LineVertexArr[2 * (i * 128 + j) + 1] = FLineVertexForInstanceDraw(
			// 			FVector2f(1, 1), 1.0f);
			// 	}
			// }
			

			// Ideal Situation: VertexBuffer with Only 2 Vertices
			// BUGGY!!! DX11 is fine, VK is WRONG???
			TResourceArray<FLineVertexForInstanceDraw> LineVertexArr;
			LineVertexArr.SetNumUninitialized(2);
			LineVertexArr[0] = FLineVertexForInstanceDraw(0);
			LineVertexArr[1] = FLineVertexForInstanceDraw(1);

			
			FRHIResourceCreateInfo VerticesCreateInfo(TEXT("FlowFieldLineVertexBuffer"), &LineVertexArr);
			VertexBuffer = RHICmdList.CreateVertexBuffer(LineVertexArr.GetResourceDataSize(), BUF_Static, VerticesCreateInfo);
			// VertexBuffer = RHICmdList.CreateBuffer(sizeof(FLineVertexForInstanceDraw) * LineVertexArr.Num(),
			//                                                         BUF_VertexBuffer | BUF_Static, 0,
			//                                                         ERHIAccess::VertexOrIndexBuffer, VerticesCreateInfo);
			// void* VoidPtr = RHICmdList.LockBuffer(VertexBuffer, 0, sizeof(FLineVertexForInstanceDraw) * LineVertexArr.Num(),
			//                                       RLM_WriteOnly);
			//
			// FMemory::Memcpy(VoidPtr, LineVertexArr.GetData(), sizeof(FLineVertexForInstanceDraw) * LineVertexArr.Num());
			// RHICmdList.UnlockBuffer(VertexBuffer);
			
			
			TResourceArray<uint32> Indices;
			Indices.SetNumUninitialized(2);
			Indices[0] = 0;
			Indices[1] = 1;
			FRHIResourceCreateInfo IndicesCreateInfo(TEXT("FlowFieldLineIndexBuffer"), &Indices);
			IndexBuffer = RHICmdList.CreateIndexBuffer(sizeof(uint32), Indices.GetResourceDataSize(), BUF_Static, IndicesCreateInfo);

			// TArray<uint32> LineIndexArr;
			// LineIndexArr.SetNumUninitialized(2);
			// LineIndexArr[0] = 0;
			// LineIndexArr[1] = 1;
			// FRHIResourceCreateInfo IndicesCreateInfo(TEXT("FlowFieldLineIndexBuffer"));
			// IndexBuffer = RHICmdList.CreateBuffer(sizeof(uint32)* LineIndexArr.Num(), BUF_IndexBuffer | BUF_Static, 0, ERHIAccess::VertexOrIndexBuffer, IndicesCreateInfo);
			//
			// VoidPtr = RHICmdList.LockBuffer(IndexBuffer, 0, sizeof(uint32) * LineIndexArr.Num(),
			// 						  RLM_WriteOnly);
			//
			// FMemory::Memcpy(VoidPtr, LineIndexArr.GetData(), sizeof(uint32) * LineIndexArr.Num());
			// RHICmdList.UnlockBuffer(IndexBuffer);

			

			// Initialize Triangle VertexBuffer & IndexBuffer
			// TResourceArray<FTriVertexForInstanceDraw> Vertices;
			// Vertices.SetNumUninitialized(4);
			//
			// TResourceArray<uint32> Indices;
			// Indices.SetNumUninitialized(6);
			//
			// Vertices[0] = FTriVertexForInstanceDraw(FVector4f(-1.0, -1.0, 0.0, 1.0));
			// Vertices[1] = FTriVertexForInstanceDraw(FVector4f(1.0, -1.0, 0.0, 1.0));
			// Vertices[2] = FTriVertexForInstanceDraw(FVector4f(1.0, 1.0, 0.0, 1.0));
			// Vertices[3] = FTriVertexForInstanceDraw(FVector4f(-1.0, 1.0, 0.0, 1.0));
			//
			// Indices[0] = 0;
			// Indices[1] = 1;
			// Indices[2] = 2;
			// Indices[3] = 0;
			// Indices[4] = 2;
			// Indices[5] = 3;
			//
			// FRHIResourceCreateInfo VerticesCreateInfo(TEXT("FF2DParticleVerts"), &Vertices);
			// VertexBuffer = RHICmdList.CreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static, VerticesCreateInfo);
			//
			// FRHIResourceCreateInfo IndicesCreateInfo(TEXT("FF2DParticleTris"), &Indices);
			// IndexBuffer = RHICmdList.CreateIndexBuffer(sizeof(uint32), Indices.GetResourceDataSize(), BUF_Static, IndicesCreateInfo);
			
		});
			

}

#endif





