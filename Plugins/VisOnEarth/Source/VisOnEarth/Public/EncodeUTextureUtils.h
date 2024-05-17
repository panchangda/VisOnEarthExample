#pragma once

#include "AssetRegistry/AssetRegistryModule.h"

// must use static ?  if use inline: LNK Error: redefinition 
static inline UTexture2D* ConvertFloat32ToUTexture2D(const TArray<float>& UValue, const TArray<float>& VValue, const TArray<float>& WValue, int width, int height) {
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

	// temp->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_RGBA32F);
	temp->UpdateResource();
	delete []datas;
	return temp;
}



inline bool SaveUTextureToAsset(const FString& PackageName, UTexture2D* InTexture)
{

	if(PackageName.IsEmpty() || !InTexture) return false;
	
	// 指定保存路径
	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();
	
	// 复制纹理到新包
	// UTexture2D* TextureToSave = Cast<UTexture2D>(StaticDuplicateObject(InTexture, Package, TEXT("LICTexture")));

	// 刷新内容浏览器
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(InTexture);
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(Package, InTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_None);

	return true;
}


inline void CreateAndSaveTextureToAsset(const FString& TextureName, const TArray<float>& UValue, const TArray<float>& VValue, const TArray<float>& WValue, int TextureWidth, int TextureHeight)
{
	// default package plugins/content/
	const FString PackageName = "/VisOnEarth/";
	FString AssetPath =  PackageName + TextureName;
	UPackage* Package = CreatePackage(*AssetPath);
	Package->FullyLoad();
	

	//创建UTexture, Name must match
	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	NewTexture->AddToRoot();            
	NewTexture->PlatformData = new FTexturePlatformData();  
	NewTexture->PlatformData->SizeX = TextureWidth;
	NewTexture->PlatformData->SizeY = TextureHeight;
	// NewTexture->PlatformData->NumSlices = 1;
	//设置像素格式
	NewTexture->PlatformData->PixelFormat = EPixelFormat::PF_A32B32G32R32F;



	FFloat32* Pixels = new FFloat32[TextureWidth * TextureHeight * 4];

	for (auto it = 0; it < TextureWidth*TextureHeight; ++it) {
		auto idx = it * 4;
		Pixels[idx] = FFloat32(UValue[it]);     // R
		Pixels[idx + 1] = FFloat32(VValue[it]); // G
		Pixels[idx + 2] = FFloat32(WValue[it]); // B
		Pixels[idx + 3] = FFloat32(1.0f); // A，假设Alpha通道完全不透明
	}
	//创建第一个MipMap
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = TextureWidth;
	Mip->SizeY = TextureHeight;

	//锁定Texture让它可以被修改
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(TextureWidth * TextureHeight * 4);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * TextureHeight * TextureWidth * 4);
	Mip->BulkData.Unlock();

	//通过以上步骤，我们完成数据的临时写入
	//执行完以下这两个步骤，编辑器中的asset会显示可以保存的状态（如果是指定Asset来获取UTexture2D的指针的情况下）
	NewTexture->Source.Init(TextureWidth, TextureHeight, 1, 1, ETextureSourceFormat::TSF_RGBA32F, (uint8*)Pixels);
	NewTexture->UpdateResource();


	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);
	//通过asset路径获取包中文件名
	FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
	//进行保存
	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	delete[] Pixels;
}

