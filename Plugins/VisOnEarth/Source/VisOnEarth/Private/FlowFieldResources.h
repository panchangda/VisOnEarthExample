#pragma once
#include "RenderGraphResources.h"

struct FFlowFieldSettings;

// Vertex Declarations
struct FLineVertexForInstanceDraw
{
	uint32 Flag;
};
class FLineVertexForInstanceDrawDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	FLineVertexForInstanceDrawDeclaration() = default;
	FLineVertexForInstanceDrawDeclaration(FLineVertexForInstanceDrawDeclaration&&) = default;
	virtual ~FLineVertexForInstanceDrawDeclaration() = default;

	void virtual InitRHI(FRHICommandListBase& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		uint16 Stride = sizeof(FLineVertexForInstanceDraw);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FLineVertexForInstanceDraw, Flag), VET_UInt, 0, Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	}

	void virtual ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};


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

struct FieldData
{
	TArray<float> UValues;
	TArray<float> VValues;
	TArray<float> WValues;
	FVector2f URange;
	FVector2f VRange;
	FVector2f WRange;

	int32 Lon, Lat, Lev;
	FVector2f LonRange;
	FVector2f LatRange;
	FVector2f LevRange;
};

// From FNiagaraPooledRWBuffer
struct FlowFieldPooledRWBuffer
{
	~FlowFieldPooledRWBuffer(){Release();}
	
	void Initialize(FRDGBuilder& GraphBuilder, const TCHAR* ResourceName, EPixelFormat PixelFormat, const void* InitialData, const FRDGBufferDesc& BufferDesc);
	void Initialize(FRDGBuilder& GraphBuilder, const TCHAR* ResourceName, EPixelFormat PixelFormat, const void* InitialData, const uint32 BytesPerElemenet, const uint32 NumElements, EBufferUsageFlags UsageFlags = EBufferUsageFlags::None);
	void Release();

	bool IsValid() const { return Buffer.IsValid(); }


	const TRefCountPtr<FRDGPooledBuffer>& GetPooledBuffer() const { return Buffer; }
	FRDGBufferRef GetOrCreateBuffer(FRDGBuilder& GraphBuilder);
	FRDGBufferSRVRef GetOrCreateSRV(FRDGBuilder& GraphBuilder);
	FRDGBufferUAVRef GetOrCreateUAV(FRDGBuilder& GraphBuilder);

	void EndGraphUsage();
	
private:
	TRefCountPtr<FRDGPooledBuffer>	Buffer;
	EPixelFormat					PixelFormat = PF_Unknown;

	// Transient for the lifetime of the GraphBuilder, call PostSimulate to clear
	FRDGBufferRef					TransientRDGBuffer = nullptr;
	FRDGBufferSRVRef				TransientRDGSRV = nullptr;
	FRDGBufferUAVRef				TransientRDGUAV = nullptr;
	
};

struct FFlowFieldResources
{
	FFlowFieldResources(){
#if FLOWFIELD_USE_COMPUTE
		
#else
		PrevParticleTextureRHI = nullptr;
		CurrParticleTextureRHI = nullptr;
		ParticleSpeedTextureRHI = nullptr;
		TexSize = FUint32Vector2(0,0); // 0: TexX, 1: TexY
#endif

		VertexBuffer = nullptr;
		IndexBuffer = nullptr;
	}
	
#if FLOWFIELD_USE_COMPUTE
	FlowFieldPooledRWBuffer ParticlePosition;
	FlowFieldPooledRWBuffer ParticleSpeed;
	FlowFieldPooledRWBuffer TrailPosition;
#else
	FTexture2DRHIRef PrevParticleTextureRHI = nullptr;
	FTexture2DRHIRef CurrParticleTextureRHI = nullptr;
	FTexture2DRHIRef ParticleSpeedTextureRHI = nullptr;

	FUint32Vector2 TexSize = FUint32Vector2(0,0); // 0: TexX, 1: TexY
	
	static constexpr uint32 MaxTrailLength = 10;
	FTexture2DRHIRef TrailParticleTexturesRHI[MaxTrailLength];

	void UpdateFlowFieldTexture(UTexture2D* FlowFieldTexture);
	void UpdateParticleTexture(UTexture2D* , UTexture2D*);
	void UpdateTrailTexture(const TArray<UTexture2D*>&, const uint32 NewTrailLength);
	void UpdateParticleSum(uint32 ParticleSum);
#endif

	
	FTexture2DRHIRef FlowFieldTexture = nullptr;
	FBufferRHIRef VertexBuffer = nullptr;
	FBufferRHIRef IndexBuffer = nullptr;
	FBufferRHIRef TmpVertexBuffer = nullptr;

	
	void InitializeVertexIndexBuffer(const FFlowFieldSettings& Settings);
	void InitializeFromAnyThread(const FFlowFieldSettings& Settings);
	void InitializeFromRenderThread(const FFlowFieldSettings& Settings);
	void InitializePooledRWBuffers(FRDGBuilder& GraphBuilder, const FFlowFieldSettings& Settings);
	void EndPooledRWBuffersGraphUsage();
	UTexture2D* InitializeFlowFieldTexture(const FFlowFieldSettings& Settings);
	FieldData LoadJson(const FString& FileName);
	bool IsValid() const;

	void PrePareUploadArray(const FFlowFieldSettings& Settings);
	TArray<FVector4f> RandomUploadArray;
	TArray<FVector4f> ZeroUploadArray;
};

