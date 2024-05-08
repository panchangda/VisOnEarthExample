#include "FlowFieldResources.h"
#include "FlowFieldSettings.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "Interfaces/IPluginManager.h"

/* FlowFieldPooledRWBuffer */
void FlowFieldPooledRWBuffer::Initialize(FRDGBuilder& GraphBuilder, const TCHAR* ResourceName, EPixelFormat InPixelFormat, const void* InitialData, const uint32 BytesPerElemenet, const uint32 NumElements, EBufferUsageFlags UsageFlags)
{
	FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateBufferDesc(BytesPerElemenet, NumElements);
	BufferDesc.Usage |= UsageFlags;
	Initialize(GraphBuilder, ResourceName, InPixelFormat, InitialData, BufferDesc);
}

void FlowFieldPooledRWBuffer::Initialize(FRDGBuilder& GraphBuilder, const TCHAR* ResourceName, EPixelFormat InPixelFormat, const void* InitialData, const FRDGBufferDesc& BufferDesc)
{
	Release();
	
	TransientRDGBuffer = GraphBuilder.CreateBuffer(BufferDesc, ResourceName);
	Buffer = GraphBuilder.ConvertToExternalBuffer(TransientRDGBuffer);
	GraphBuilder.QueueBufferUpload(TransientRDGBuffer, InitialData, BufferDesc.NumElements * BufferDesc.BytesPerElement);
	PixelFormat = InPixelFormat;
	TransientRDGBuffer = nullptr;
}
void FlowFieldPooledRWBuffer::Release()
{
	Buffer.SafeRelease();
	PixelFormat = PF_Unknown;
}

FRDGBufferRef FlowFieldPooledRWBuffer::GetOrCreateBuffer(FRDGBuilder& GraphBuilder)
{
	check(IsValid());
	if (TransientRDGBuffer == nullptr)
	{
		TransientRDGBuffer = GraphBuilder.RegisterExternalBuffer(Buffer);
	}
	else
	{
		check(GraphBuilder.FindExternalBuffer(Buffer) != nullptr);
	}
	return TransientRDGBuffer;
}

FRDGBufferSRVRef FlowFieldPooledRWBuffer::GetOrCreateSRV(FRDGBuilder& GraphBuilder)
{
	check(IsValid());
	if (TransientRDGSRV == nullptr)
	{
		TransientRDGSRV = GraphBuilder.CreateSRV(GetOrCreateBuffer(GraphBuilder), PixelFormat);
	}
	else
	{
		check(GraphBuilder.FindExternalBuffer(Buffer) != nullptr);
	}
	return TransientRDGSRV;
}

FRDGBufferUAVRef FlowFieldPooledRWBuffer::GetOrCreateUAV(FRDGBuilder& GraphBuilder)
{
	check(IsValid());
	if (TransientRDGUAV == nullptr)
	{
		TransientRDGUAV = GraphBuilder.CreateUAV(GetOrCreateBuffer(GraphBuilder), PixelFormat);
	}
	else
	{
		check(GraphBuilder.FindExternalBuffer(Buffer) != nullptr);
	}
	return TransientRDGUAV;
}

void FlowFieldPooledRWBuffer::EndGraphUsage()
{
	TransientRDGBuffer = nullptr;
	TransientRDGSRV = nullptr;
	TransientRDGUAV = nullptr;
}


TResourceArray<FVector4f> ParallellyInitialize(const int32& Sum, const FVector4f& Value, const bool& UseRandom = false)
{
	TResourceArray<FVector4f> ArrToInitialize;
	ArrToInitialize.SetNumUninitialized(Sum);
	if (UseRandom)
	{
		ParallelFor(Sum, [&](int32 Index)
		{
			check(!IsInGameThread() && !IsInRenderingThread());
			ArrToInitialize[Index] = FVector4f(
				FMath::RandRange(0.f, 1.f),
				FMath::RandRange(0.f, 1.f),
				0.0f, //FMath::RandRange(0.f, 1.f)
				1.0f);
		});
	}
	else
	{
		ParallelFor(Sum, [&](int32 Index)
		{
			check(!IsInGameThread() && !IsInRenderingThread());
			ArrToInitialize[Index] = Value;
		});
	}

	// Will RVO Optimize this?
	return ArrToInitialize;
}

TArray<FVector4f> ParallellyInitializeTArray(const int32& Sum, const FVector4f& Value, const bool& UseRandom = false)
{
	TArray<FVector4f> ArrToInitialize;
	ArrToInitialize.SetNumUninitialized(Sum);
	if (UseRandom)
	{
		ParallelFor(Sum, [&](int32 Index)
		{
			check(!IsInGameThread() && !IsInRenderingThread());
			ArrToInitialize[Index] = FVector4f(
				FMath::RandRange(0.f, 1.f),
				FMath::RandRange(0.f, 1.f),
				0.0f, //FMath::RandRange(0.f, 1.f)
				1.0f);
		});
	}
	else
	{
		ParallelFor(Sum, [&](int32 Index)
		{
			check(!IsInGameThread() && !IsInRenderingThread());
			ArrToInitialize[Index] = Value;
		});
	}

	// Will RVO Optimize this?
	return ArrToInitialize;
}

void FFlowFieldResources::PrePareUploadArray(const FFlowFieldSettings& Settings)
{
	// Async: How to wait for Async Task?
	// AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Settings]()
	// {
	// 	// Parallelly Initialize Random Particle Position Array
	// 	RandomUploadArray = ParallellyInitializeTArray(Settings.ParticlesSum,
	// 	                                               FVector4f(),
	// 	                                               true);
	// 	// Parallelly Initialize Zero Upload Array 
	// 	ZeroUploadArray = ParallellyInitializeTArray(
	// 		Settings.ParticlesSum * Settings.TrailLength == 0 ? 1 : Settings.TrailLength,
	// 		FVector4f(0.0f, 0.0f, 0.0f, 0.0f),
	// 		false);
	// });

	RandomUploadArray.SetNumUninitialized(Settings.ParticlesSum);
	for(int i=0;i<Settings.ParticlesSum;i++)
	{
		RandomUploadArray[i] = FVector4f(FMath::RandRange(Settings.DataLonRange.X, Settings.DataLatRange.Y),
			FMath::RandRange(Settings.DataLatRange.X, Settings.DataLatRange.Y),FMath::RandRange(Settings.DataLevRange.X, Settings.DataLevRange.Y) , 1.0f);
	}
	int Sum = Settings.TrailLength == 0 ? Settings.ParticlesSum : Settings.ParticlesSum * Settings.TrailLength;
	ZeroUploadArray.SetNumUninitialized(Sum);
	for(int i=0;i<Sum;i++)
	{
		ZeroUploadArray[i] = FVector4f(0.0f,0.0f,0.0f,0.0f);
	}
}
void FFlowFieldResources::InitializeFromAnyThread(const FFlowFieldSettings& Settings)
{
	InitializeFlowFieldTexture(Settings);
	
	InitializeVertexIndexBuffer(Settings);
}
void FFlowFieldResources::InitializeFromRenderThread(const FFlowFieldSettings& Settings)
{
	
}
void FFlowFieldResources::EndPooledRWBuffersGraphUsage()
{
	ParticlePosition.EndGraphUsage();
	ParticleSpeed.EndGraphUsage();
	TrailPosition.EndGraphUsage();
}

void FFlowFieldResources::InitializePooledRWBuffers(FRDGBuilder& GraphBuilder, const FFlowFieldSettings& Settings)
{
	PrePareUploadArray(Settings);

	// Initialize FlowField RW Pooled Buffers
	ParticlePosition.Initialize(GraphBuilder, TEXT("ParticlePositionBuffer"),
	                            PF_A32B32G32R32F, RandomUploadArray.GetData(), sizeof(FVector4f),
	                            Settings.ParticlesSum);
	ParticleSpeed.Initialize(GraphBuilder, TEXT("ParticleSpeedBuffer"),
	                         PF_A32B32G32R32F, ZeroUploadArray.GetData(), sizeof(FVector4f), Settings.ParticlesSum);
	TrailPosition.Initialize(GraphBuilder, TEXT("TrailPositionBuffer"),
	                         PF_A32B32G32R32F, ZeroUploadArray.GetData(), sizeof(FVector4f), ZeroUploadArray.Num());
}

void FFlowFieldResources::InitializeVertexIndexBuffer(const FFlowFieldSettings& Settings)
{
	ENQUEUE_RENDER_COMMAND(GenerateFlowParticlesCommand)
	([this](FRHICommandListImmediate& RHICmdList)
	{
		// Initialize Vertex&Index Buffer
		TResourceArray<FLineVertexForInstanceDraw> LineVertexArr;
		LineVertexArr.SetNumUninitialized(2);
		LineVertexArr[0] = FLineVertexForInstanceDraw(0);
		LineVertexArr[1] = FLineVertexForInstanceDraw(1);
	
	
		FRHIResourceCreateInfo VerticesCreateInfo(TEXT("FlowFieldLineVertexBuffer"), &LineVertexArr);
		VertexBuffer = RHICmdList.CreateVertexBuffer(LineVertexArr.GetResourceDataSize(), BUF_Static,
		                                             VerticesCreateInfo);
	
		TResourceArray<uint32> Indices;
		Indices.SetNumUninitialized(2);
		Indices[0] = 0;
		Indices[1] = 1;
		FRHIResourceCreateInfo IndicesCreateInfo(TEXT("FlowFieldLineIndexBuffer"), &Indices);
		IndexBuffer = RHICmdList.CreateIndexBuffer(sizeof(uint32), Indices.GetResourceDataSize(), BUF_Static,
		                                           IndicesCreateInfo);


		TResourceArray<FTriVertexForInstanceDraw> TmpLineVertexArr;
		TmpLineVertexArr.SetNumUninitialized(4);
		TmpLineVertexArr[0] = FTriVertexForInstanceDraw(FVector4f(0.0, 0.0, 1.0, 1.0));
		TmpLineVertexArr[1] = FTriVertexForInstanceDraw(FVector4f(1.0, 1.0, 1.0, 1.0));
		TmpLineVertexArr[2] = FTriVertexForInstanceDraw(FVector4f(1.0, 0.0, 1.0, 1.0));
		TmpLineVertexArr[3] = FTriVertexForInstanceDraw(FVector4f(0.0, 1.0, 1.0, 1.0));

		FRHIResourceCreateInfo TmpVerticesCreateInfo(TEXT("TmpLineVertexBuffer"), &TmpLineVertexArr);
		TmpVertexBuffer = RHICmdList.CreateVertexBuffer(TmpLineVertexArr.GetResourceDataSize(), BUF_Static,
		                                                TmpVerticesCreateInfo);

// FRHIResourceCreateInfo VerticesCreateInfo(TEXT("TmpVertexBuffer"));
// FBufferRHIRef TmpVertexBuffer = RHICmdList.CreateBuffer(
// 	sizeof(FTriVertexForInstanceDraw) * LineVertexArr.Num(),
// 	BUF_VertexBuffer | BUF_Static, 0,
// 	ERHIAccess::VertexOrIndexBuffer, VerticesCreateInfo);
// void* VoidPtr = RHICmdList.LockBuffer(TmpVertexBuffer, 0,
// 									  sizeof(FTriVertexForInstanceDraw) * LineVertexArr.Num(),
// 									  RLM_WriteOnly);
//
// FMemory::Memcpy(VoidPtr, LineVertexArr.GetData(), sizeof(FTriVertexForInstanceDraw) * LineVertexArr.Num());
// RHICmdList.UnlockBuffer(TmpVertexBuffer);
	});

	// ENQUEUE_RENDER_COMMAND(TestFlowParticlesVertexIndexBuffer)
	// ([this, Settings](FRHICommandListImmediate& RHICmdList)
	// {
	// 	// Initialize Vertex&Index Buffer
	// 	TResourceArray<FLineVertexForInstanceDraw> LineVertexArr;
	// 	LineVertexArr.SetNumUninitialized(Settings.ParticlesSum * 2);
	// 	for (int32 i = 0; i < Settings.ParticlesSum; i++)
	// 	{
	// 		LineVertexArr[2 * i] = FLineVertexForInstanceDraw(0);
	// 		LineVertexArr[2 * i + 1] = FLineVertexForInstanceDraw(1);
	// 	}
	//
	//
	// 	FRHIResourceCreateInfo VerticesCreateInfo(TEXT("FlowFieldLineVertexBuffer"), &LineVertexArr);
	// 	VertexBuffer = RHICmdList.CreateVertexBuffer(LineVertexArr.GetResourceDataSize(), BUF_Static,
	// 	                                             VerticesCreateInfo);
	//
	// 	TResourceArray<uint32> Indices;
	// 	Indices.SetNumUninitialized(2);
	// 	Indices[0] = 0;
	// 	Indices[1] = 1;
	// 	FRHIResourceCreateInfo IndicesCreateInfo(TEXT("FlowFieldLineIndexBuffer"), &Indices);
	// 	IndexBuffer = RHICmdList.CreateIndexBuffer(sizeof(uint32), Indices.GetResourceDataSize(), BUF_Static,
	// 	                                           IndicesCreateInfo);
	// });
	
}


UTexture2D* ConvertFloat32ToUTexture2D(const TArray<float>& UValue, const TArray<float>& VValue, const TArray<float>& WValue, int width, int height) {
	if (UValue.IsEmpty() || VValue.IsEmpty() || WValue.IsEmpty()) return nullptr;

	FFloat32* datas = new FFloat32[height * width * 4];

	for (auto it = 0; it < width*height; ++it) {
		auto idx = it * 4;
		datas[idx] = FFloat32(UValue[it]);     // R
		datas[idx + 1] = FFloat32(VValue[it]); // G
		datas[idx + 2] = FFloat32(WValue[it]); // B
		datas[idx + 3] = FFloat32(1.0f); // A，假设Alpha通道完全不透明
	}
	
	UTexture2D* temp = UTexture2D::CreateTransient(width, height, PF_A32B32G32R32F);
	temp->Filter = TF_Bilinear;
#if WITH_EDITORONLY_DATA
	temp->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	temp->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
#endif // WITH_EDITORONLY_DATA
	// void* TextureData = temp->Source.LockMip(0);
	void* TextureData = temp->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memmove(TextureData, datas, height * width * 4 * sizeof(FFloat32));
	temp->GetPlatformData()->Mips[0].BulkData.Unlock();
	// temp->Source.UnlockMip(0);
	// 更新纹理数据
	temp->UpdateResource();
	delete []datas;
	return temp;
}

UTexture2D* FFlowFieldResources::InitializeFlowFieldTexture(const FFlowFieldSettings& Settings)
{
	FieldData InFieldData = LoadJson(Settings.JsonDataFile);
	
	// Ugly Implementation: Must Replace With Async & Parallel For
	UTexture2D* tex;
	if (IsInRenderingThread())
	{
		AsyncTask(ENamedThreads::GameThread, [this, InFieldData, &tex]()
		{
			tex = ConvertFloat32ToUTexture2D(InFieldData.UValues, InFieldData.VValues, InFieldData.WValues, InFieldData.Lon, InFieldData.Lat);

			ENQUEUE_RENDER_COMMAND(GenerateFlowFieldTextureCommand)([this, tex](FRHICommandListImmediate& RHICmdList)
			{
				// Why Cannot use capture like this: FlowFieldTexture = this->FlowFieldTexture ???
				this->FlowFieldTexture = tex->GetResource()->GetTexture2DRHI();
			});
		});
	}else
	{
		tex = ConvertFloat32ToUTexture2D(InFieldData.UValues, InFieldData.VValues, InFieldData.WValues, InFieldData.Lon, InFieldData.Lat);
		ENQUEUE_RENDER_COMMAND(GenerateFlowFieldTextureCommand)([this, tex](FRHICommandListImmediate& RHICmdList)
		{
			// Why Cannot use capture like this: FlowFieldTexture = this->FlowFieldTexture ???
			this->FlowFieldTexture = tex->GetResource()->GetTexture2DRHI();
		});
	}
	return tex;
}





FieldData FFlowFieldResources::LoadJson(const FString& FileName)
{
	FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("VisOnEarth"))->GetBaseDir();
	FString DataFilePath = FPaths::Combine(*PluginDir, FString::Format(TEXT("Resources/NCData/{0}"), {FileName}));
	// 确保路径正确生成后再进行下一步操作，比如打开文件
	UE_LOG(LogTemp, Log, TEXT("Opening JSON Data File: %s"), *DataFilePath);

	// load json
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *DataFilePath);
	
	// process json
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	
	int32 Lon, Lat, Lev;
	Lon = Lat  = Lev = 1;
	TArray<float> UValues;
	TArray<float> VValues;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// read dimensions
		const TSharedPtr<FJsonObject>* DimensionsObject;
		if (JsonObject->TryGetObjectField(TEXT("dimensions"), DimensionsObject))
		{
			// 读取dimensions下的每个字段
			
			(*DimensionsObject)->TryGetNumberField(TEXT("lon"), Lon);
			(*DimensionsObject)->TryGetNumberField(TEXT("lat"), Lat);
			(*DimensionsObject)->TryGetNumberField(TEXT("lev"), Lev);

			// 现在你有了lon, lat和lev的值
			UE_LOG(LogTemp, Log, TEXT("Reading Json Value lon:%d lat:%d lev:%d"),Lon,Lat,Lev);
		}
		// 获取U对象
		const TSharedPtr<FJsonObject>* UObject;
		if (JsonObject->TryGetObjectField(TEXT("U"), UObject))
		{
			// 读取array数组
			const TArray<TSharedPtr<FJsonValue>>* UArray;
			if ((*UObject)->TryGetArrayField(TEXT("array"), UArray))
			{

				// 遍历数组，读取每个值
				for (TSharedPtr<FJsonValue> Value : *UArray)
				{
					UValues.Add(Value->AsNumber());
				}

				// 现在你有了一个包含所有lon值的数组LonValues
				UE_LOG(LogTemp, Log, TEXT("Reading Json Value UArray Success"));
			}
		}
		const TSharedPtr<FJsonObject>* VObject;
		if (JsonObject->TryGetObjectField(TEXT("V"), VObject))
		{
			// 读取array数组
			const TArray<TSharedPtr<FJsonValue>>* VArray;
			if ((*VObject)->TryGetArrayField(TEXT("array"), VArray))
			{

				// 遍历数组，读取每个值
				for (TSharedPtr<FJsonValue> Value : *VArray)
				{
					VValues.Add(Value->AsNumber());
				}

				// 现在你有了一个包含所有lon值的数组LonValues
				UE_LOG(LogTemp, Log, TEXT("Reading Json Value VArray Success"));
			}
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON Data File"));
	}

	TArray<float> EmptyFloatArray;
	EmptyFloatArray.Init(0.0f, UValues.Num());
	
	return {
		.UValues = UValues, .VValues = VValues, .WValues =EmptyFloatArray,
		.URange = FVector2f(), .VRange = FVector2f(), .WRange = FVector2f(),
		.Lon = Lon, .Lat = Lat, .Lev = Lev,
		.LonRange = FVector2f(), .LatRange = FVector2f(),.LevRange = FVector2f()
	};
}

bool FFlowFieldResources::IsValid() const
{
	return 
#if FLOWFIELD_USE_COMPUTE
	ParticlePosition.IsValid()
	&& ParticleSpeed.IsValid()
	&& TrailPosition.IsValid()
#else
	
#endif
	&& VertexBuffer.IsValid()
	&& IndexBuffer.IsValid()
	&& FlowFieldTexture.IsValid();
	
	return false;
}

// void AFlowFieldActor::SetTrailTextures(const int32 Width, const int32 Height, const int32 NewTrailLength)
// {
// 	if(!renderer.IsValid()) return;
// 	
// 	TArray<FVector4f> TrailParticleArray;
// 	TrailParticleArray.Init(FVector4f(0.0, 0.0, 0.0,
// 		0.0), // w as flag: 0 means particle not drawable
// 		Width*Height);
// 	// intialize all trail textures
// 	TrailParticleTexture.SetNum(FFlowFieldRenderer::MaxTrailLength);
// 	for(int32 i=0;i<FFlowFieldRenderer::MaxTrailLength;i++)
// 	{
// 		TrailParticleTexture[i] = ConvertFloat32ToUTexture2D(TrailParticleArray, Width, Height);
// 	}
//
// 	// Update Corresponding Render Thread Resources
// 	if(renderer.IsValid())
// 	{
// 		renderer->UpdateTrailTexture(TrailParticleTexture, NewTrailLength);
// 	}else
// 	{
// 		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
// 	}
// }
// void AFlowFieldActor::SetParticlesTexture(const int32 Width, const int32 Height)
// {
// 	TArray<FVector4f> CurrParticleArray;
// 	TArray<FVector4f> ParticleSpeedArray;
// 	ParticleSpeedArray.Reserve(Width*Height);
// 	CurrParticleArray.Reserve(Width*Height);
//
//
// 	ParticleSpeedArray.Reserve(Width*Height);
// 	for(int i=0;i<Width*Height;i++)
// 	{
// 		CurrParticleArray.Emplace(FVector4f(
// 			FMath::RandRange(0.0, 1.0),
// 			FMath::RandRange(0.0, 1.0),
// 			FMath::RandRange(0.0, 1.0),
// 			1));
// 		ParticleSpeedArray.Emplace(FVector4f(
// 			0.0, 0.0, 0.0, 1.0
// 		));
// 	}
// 	
// 	CurrParticleTexture = ConvertFloat32ToUTexture2D(CurrParticleArray, Width, Height);
// 	ParticleSpeedTexture = ConvertFloat32ToUTexture2D(ParticleSpeedArray, Width, Height);
//
// 	
//
// 	
// 	// Update Corresponding Render Thread Resources
// 	if(renderer.IsValid())
// 	{
// 		renderer->UpdateParticleTexture(CurrParticleTexture, ParticleSpeedTexture);
// 	}else
// 	{
// 		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
// 	}
// 	
// }
// void AFlowFieldActor::SetParticlesSum(const int32 ParticleSum)
// {
// 	if(renderer.IsValid())
// 	{
// 		renderer->UpdateParticleSum(ParticleSum);
// 	}else
// 	{
// 		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
// 	}
//
// }