#pragma once


static inline TArray<float> LoadDatFile(const FString& FilePath, const int& LongitudeNum, const int& LatitudeNum)
{
	TArray<float> Data;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 读取第一个文件
	IFileHandle* FileHandle = PlatformFile.OpenRead(*FilePath);
	if (!FileHandle)
		return Data;


	Data.SetNum(LongitudeNum * LatitudeNum);
	float Temp;
	FileHandle->Read((uint8*)&Temp, sizeof(float)); // 读取并丢弃一个浮点数
	FileHandle->Read((uint8*)Data.GetData(), sizeof(float) * LongitudeNum * LatitudeNum);
	delete FileHandle; // 关闭文件句柄
	

	return Data;	
}

