// Fill out your copyright notice in the Description page of Project Settings.


#include "VortexActor.h"

#include "CesiumGeoreference.h"
#include "DataIOUtil.h"
#include "EncodeUTextureUtils.h"
#include "GeographyUtil.h"
#include "Interfaces/IPluginManager.h"

static const int VarA = 3;
static const int VarB = 2;
#define longitudeNum LongitudeNum
#define latitudeNum LatitudeNum


// Sets default values
AVortexActor::AVortexActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Load default static mesh (sphere)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAssetFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMeshAssetFinder.Succeeded())
    {
        SphereMeshAsset = SphereMeshAssetFinder.Object;
    }
}

// Called when the game starts or when spawned
void AVortexActor::BeginPlay()
{
	Super::BeginPlay(); 
	VortexData = InitializeData();
    VortexPoint = DetectVortex(VortexData);

    // VortexPoint is already in coordinate llh, l should transform from [-180, 180] to [0, 360]
    TArray<FVector> VortexPoint3D;
    VortexPoint3D.SetNum(VortexPoint.Num());
    for(int i=0;i<VortexPoint3D.Num();i++)
    {
        VortexPoint3D[i] = DefaultGeoReference->TransformLongitudeLatitudeHeightPositionToUnreal(
            NormalizedToRanged(LonRange, LatRange, LevRange,
                // Strange Divide: Copy From previous osg vertex implementation
                FVector(  VortexPoint[i].Y / LongitudeNum, VortexPoint[i].X / LatitudeNum, 0.0)));
    }
    SpawnSpheresAtLocations(VortexPoint3D, DefaultVortexMaterial);
}

// Called every frame
void AVortexActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


TArray<FVector2D> AVortexActor::InitializeData()
{

	FString UDataFileName = "U.DAT";
	FString VDataFileName = "V.DAT";
	
	FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("VisOnEarth"))->GetBaseDir();
	FString UDataFilePath = FPaths::Combine(*PluginDir, FString::Format(TEXT("Resources/NCData/{0}"), {UDataFileName}));
	FString VDataFilePath = FPaths::Combine(*PluginDir, FString::Format(TEXT("Resources/NCData/{0}"), {VDataFileName}));
	
	TArray<float> UValues, VValues, WValues;
	
	UValues = LoadDatFile(UDataFilePath, LongitudeNum, LatitudeNum);
	VValues = LoadDatFile(VDataFilePath, LongitudeNum, LatitudeNum);
	WValues.SetNumZeroed(LongitudeNum * LatitudeNum);

	FlowFieldTexture2D = ConvertFloat32ToUTexture2D(UValues, VValues, WValues, LongitudeNum, LatitudeNum);

	TArray<FVector2D> _VortexData;
    _VortexData.SetNum( LongitudeNum* LatitudeNum);
    for(int i = 0 ; i < _VortexData.Num(); i++)
    {
        _VortexData[i] =  FVector2D(UValues[i], VValues[i]);
    }
	return _VortexData;
    
}


TArray<FVector2D> AVortexActor::DetectVortex(const TArray<FVector2D>& InData)
{

	TArray<FVector2D> ans, ans1;
	ans = CounterClockWise(InData);
	ans1 = Clockwise(InData);
    ans.Append(ans1);
	return ans;
		
}

// Function to spawn spheres at multiple locations with VarA specific material
void AVortexActor::SpawnSpheresAtLocations(const TArray<FVector>& Locations, UMaterialInterface* Material)
{
    if (SphereMeshAsset)
    {
        for (const FVector& Location : Locations)
        {
            // Create VarA new static mesh component for each sphere
            UStaticMeshComponent* SphereMeshComponent = NewObject<UStaticMeshComponent>(this);
            SphereMeshComponent->SetStaticMesh(SphereMeshAsset);

            // Set material if provided
            if (Material)
            {
                SphereMeshComponent->SetMaterial(0, Material);
            }

            // Attach the mesh component to the root component and set its location
            SphereMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
            SphereMeshComponent->SetWorldLocation(Location);
            
            SphereMeshComponent->SetWorldScale3D(SphereScaleCoef);
            // Register the component to the world
            SphereMeshComponent->RegisterComponent();
        }
    }
}



bool AVortexActor::IsValidLine(const FVector2D& Line0, const FVector2D& Line1, const TArray<FVector2D>& speed)
{
    bool ok = true;
    if (Line0.X >= latitudeNum || Line0.X < 0 || Line0.Y >= longitudeNum || Line0.Y < 0
        || Line1.X >= latitudeNum || Line1.X < 0 || Line1.Y >= longitudeNum || Line1.Y < 0) return false;
    if (Line0.X == Line1.X)
    {
        for (int i = Line0.Y; i <= Line1.Y; ++i)
        {
            if (speed[Line0.X * longitudeNum + i].X == 9999 || speed[Line0.X * longitudeNum + i].Y == 9999)
            {
                ok = false;
                break;
            }
        }
    }
    else if (Line0.Y == Line1.Y)
    {
        for (int i = Line0.X; i <= Line1.X; i++)
        {
            if (speed[i * longitudeNum + Line0.Y].X == 9999 || speed[i * longitudeNum + Line0.Y].Y == 9999)
            {
                ok = false;
                break;
            }
        }
    }
    return ok;
}

bool AVortexActor::IsValidQuad(const FVector2D& InVec, int q, const TArray<FVector2D>& speed)
{
    bool ok = true;
    if (InVec.X + q >= latitudeNum || InVec.X - q < 0 || InVec.Y + q >= longitudeNum || InVec.Y - q < 0) return false;
    for (int i = -q; i <= q; i++) // x
    {
        for (int j = -q; j <= q; ++j) // y
        {
            if (speed[(InVec.X + i) * longitudeNum + InVec.Y + j].X == 9999 || speed[(InVec.X + i) * longitudeNum + InVec.Y + j].Y == 9999)
            {
                ok = false;
                break;
            }
        }
        if (!ok) break;
    }
    return ok;
}

int SignXY(const FVector2D& InVec)
{
    if (InVec.X >= 0 && InVec.Y >= 0) return 0;
    if (InVec.X <= 0 && InVec.Y >= 0) return 1;
    if (InVec.X <= 0 && InVec.Y <= 0) return 2;
    return 3;
}

TArray<FVector2D> AVortexActor::Clockwise(const TArray<FVector2D>& speed)
{
    TArray<FVector2D> num1;
    for (int i = 0; i < latitudeNum; i++)
    {
        for (int j = 0; j < longitudeNum - 1; j++)
        {
            if (!IsValidLine(FVector2D(i, j - VarA), FVector2D(i, j + 1 + VarA), speed)) continue;
            if (speed[i * longitudeNum + j].Y >= 0 && speed[i * longitudeNum + j + 1].Y <= 0)
            {
                if (speed[i * longitudeNum + j - VarA].Y >= 0 && speed[i * longitudeNum + j + 1 + VarA].Y <= 0
                    && FMath::Abs(speed[i * longitudeNum + j].Y) < FMath::Abs(speed[i * longitudeNum + j - VarA].Y)
                    && FMath::Abs(speed[i * longitudeNum + j + 1].Y) < FMath::Abs(speed[i * longitudeNum + j + 1 + VarA].Y))
                {
                    num1.Add(FVector2D(i, j));
                    num1.Add(FVector2D(i, j + 1));
                }
            }
        }
    }

    TArray<FVector2D> num2;
    for (int i = 0; i < num1.Num(); i += 2)
    {
        if (!IsValidLine(FVector2D(num1[i].X - VarA, num1[i].Y), FVector2D(num1[i].X + VarA, num1[i].Y), speed) ||
            !IsValidLine(FVector2D(num1[i].X - VarA, num1[i].Y + 1), FVector2D(num1[i].X + VarA, num1[i].Y + 1), speed))
        {
            continue;
        }

        if (speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y].X >= 0
            && speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y + 1].X >= 0
            && FMath::Abs(speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y].X)
            && FMath::Abs(speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y + 1].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y + 1].X)
            && speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y].X <= 0
            && speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y + 1].X <= 0
            && FMath::Abs(speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y].X)
            && FMath::Abs(speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y + 1].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y + 1].X))
        {
            num2.Add(num1[i]);
            num2.Add(num1[i + 1]);
        }
    }

    TArray<FVector2D> nums3;
    TSet<int32> in_nums2;
    for (int i = 0; i < num2.Num(); i++)
    {
        in_nums2.Add(num2[i].X * longitudeNum + num2[i].Y);
    }
    TSet<int32> vortexp;
    for (int c = 0; c < num2.Num(); ++c)
    {
        FVector2D min_point(0, 0);
        FVector2D cur = num2[c];
        float minLength = 1e9;
        if (!IsValidQuad(cur, VarB, speed)) continue;
        while (1)
        {
            for (int i = -VarB; i <= VarB; i++)
            {
                for (int j = -VarB; j <= VarB; ++j)
                {
                    if (speed[(cur.X + i) * longitudeNum + cur.Y + j].Size() < minLength)
                    {
                        minLength = speed[(cur.X + i) * longitudeNum + cur.Y + j].Size();
                        min_point = FVector2D(cur.X + i, cur.Y + j);
                    }
                }
            }
            if (cur.X == min_point.X && cur.Y == min_point.Y)
            {
                if (!vortexp.Contains(cur.X * longitudeNum + cur.Y))
                {
                    vortexp.Add(cur.X * longitudeNum + cur.Y);
                    nums3.Add(cur);
                }
                break;
            }
            if (!in_nums2.Contains(min_point.X * longitudeNum + min_point.Y))
            {
                break;
            }
            else
            {
                cur = min_point;
            }
        }
    }

    TArray<FVector2D> ans;
    for (int c = 0; c < nums3.Num(); c++)
    {
        FVector2D cur = nums3[c];
        if (cur.X + VarA - 1 >= latitudeNum || cur.X - VarA + 1 < 0 || cur.Y + VarA - 1 >= longitudeNum || cur.Y - VarA + 1 < 0) continue;
        TArray<FVector2D> ccw;
        for (int y = cur.Y + VarA - 1; y >= cur.Y - VarA + 1; --y)
        {
            ccw.Add(FVector2D(cur.X + VarA - 1, y));
        }
        for (int x = cur.X + VarA - 1; x >= cur.X - VarA + 1; --x)
        {
            ccw.Add(FVector2D(x, cur.Y - VarA + 1));
        }
        for (int y = cur.Y - VarA + 1; y <= cur.Y + VarA - 1; ++y)
        {
            ccw.Add(FVector2D(cur.X - VarA + 1, y));
        }
        for (int x = cur.X - VarA + 1; x <= cur.X + VarA - 1; ++x)
        {
            ccw.Add(FVector2D(x, cur.Y + VarA - 1));
        }

        bool flag = true;
        int signXY0 = SignXY(speed[ccw[0].X * longitudeNum + ccw[0].Y]);
        for (int i = 1; i < ccw.Num(); ++i)
        {
            int signXY1 = SignXY(speed[ccw[i].X * longitudeNum + ccw[i].Y]);
            if (signXY0 != signXY1 && (signXY0 + 1) % 4 != signXY1)
            {
                flag = false;
                break;
            }
            signXY0 = signXY1;
        }
        if (flag)
            ans.Add(cur);
    }
    return ans;
}

TArray<FVector2D> AVortexActor::CounterClockWise(const TArray<FVector2D>& speed)
{
    TArray<FVector2D> num1;
    for (int i = 0; i < latitudeNum; i++)
    {
        for (int j = 0; j < longitudeNum - 1; j++)
        {
            if (!IsValidLine(FVector2D(i, j - VarA), FVector2D(i, j + 1 + VarA), speed)) continue;
            if (speed[i * longitudeNum + j].Y <= 0 && speed[i * longitudeNum + j + 1].Y >= 0)
            {
                if (speed[i * longitudeNum + j - VarA].Y <= 0 && speed[i * longitudeNum + j + 1 + VarA].Y >= 0
                    && FMath::Abs(speed[i * longitudeNum + j].Y) < FMath::Abs(speed[i * longitudeNum + j - VarA].Y)
                    && FMath::Abs(speed[i * longitudeNum + j + 1].Y) < FMath::Abs(speed[i * longitudeNum + j + 1 + VarA].Y))
                {
                    num1.Add(FVector2D(i, j));
                    num1.Add(FVector2D(i, j + 1));
                }
            }
        }
    }

    TArray<FVector2D> num2;
    for (int i = 0; i < num1.Num(); i += 2)
    {
        if (!IsValidLine(FVector2D(num1[i].X - VarA, num1[i].Y), FVector2D(num1[i].X + VarA, num1[i].Y), speed) ||
            !IsValidLine(FVector2D(num1[i].X - VarA, num1[i].Y + 1), FVector2D(num1[i].X + VarA, num1[i].Y + 1), speed))
        {
            continue;
        }

        if (speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y].X <= 0
            && speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y + 1].X <= 0
            && FMath::Abs(speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y].X)
            && FMath::Abs(speed[(num1[i].X + VarA) * longitudeNum + num1[i].Y + 1].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y + 1].X)
            && speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y].X >= 0
            && speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y + 1].X >= 0
            && FMath::Abs(speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y].X)
            && FMath::Abs(speed[(num1[i].X - VarA) * longitudeNum + num1[i].Y + 1].X) > FMath::Abs(speed[num1[i].X * longitudeNum + num1[i].Y + 1].X))
        {
            num2.Add(num1[i]);
            num2.Add(num1[i + 1]);
        }
    }

    TArray<FVector2D> nums3;
    TSet<int32> in_nums2;
    for (int i = 0; i < num2.Num(); i++)
    {
        in_nums2.Add(num2[i].X * longitudeNum + num2[i].Y);
    }
    TSet<int32> vortexp;
    for (int c = 0; c < num2.Num(); ++c)
    {
        FVector2D min_point(0, 0);
        FVector2D cur = num2[c];
        float minLength = 1e9;
        if (!IsValidQuad(cur, VarB, speed)) continue;
        while (1)
        {
            for (int i = -VarB; i <= VarB; i++)
            {
                for (int j = -VarB; j <= VarB; ++j)
                {
                    if (speed[(cur.X + i) * longitudeNum + cur.Y + j].Size() < minLength)
                    {
                        minLength = speed[(cur.X + i) * longitudeNum + cur.Y + j].Size();
                        min_point = FVector2D(cur.X + i, cur.Y + j);
                    }

                }
            }
            if (cur.X == min_point.X && cur.Y == min_point.Y)
            {
                if (!vortexp.Contains(cur.X * longitudeNum + cur.Y))
                {
                    vortexp.Add(cur.X * longitudeNum + cur.Y);
                    nums3.Add(cur);
                }
                break;
            }
            if (!in_nums2.Contains(min_point.X * longitudeNum + min_point.Y))
            {
                break;
            }
            else
            {
                cur = min_point;
            }
        }
    }

    TArray<FVector2D> ans;
    for (int c = 0; c < nums3.Num(); c++)
    {
        FVector2D cur = nums3[c];
        if (cur.X + VarA - 1 >= latitudeNum || cur.X - VarA + 1 < 0 || cur.Y + VarA - 1 >= longitudeNum || cur.Y - VarA + 1 < 0) continue;
        TArray<FVector2D> ccw;
        for (int y = cur.Y + VarA - 1; y >= cur.Y - VarA + 1; --y)
        {
            ccw.Add(FVector2D(cur.X + VarA - 1, y));
        }
        for (int x = cur.X + VarA - 1; x >= cur.X - VarA + 1; --x)
        {
            ccw.Add(FVector2D(x, cur.Y - VarA + 1));
        }
        for (int y = cur.Y - VarA + 1; y <= cur.Y + VarA - 1; ++y)
        {
            ccw.Add(FVector2D(cur.X - VarA + 1, y));
        }
        for (int x = cur.X - VarA + 1; x <= cur.X + VarA - 1; ++x)
        {
            ccw.Add(FVector2D(x, cur.Y + VarA - 1));
        }

        bool flag = true;
        int signXY0 = SignXY(speed[ccw[0].X * longitudeNum + ccw[0].Y]);
        for (int i = 1; i < ccw.Num(); ++i)
        {
            int signXY1 = SignXY(speed[ccw[i].X * longitudeNum + ccw[i].Y]);

            if (signXY0 != signXY1 && (signXY0 + 1) % 4 != signXY1)
            {
                flag = false;
                break;
            }
            signXY0 = signXY1;
        }
        if (flag)
            ans.Add(cur);
    }
    return ans;
}

