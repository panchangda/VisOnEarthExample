#pragma once
#include "RenderGraph.h"

#include "CoreMinimal.h"
#include "FlowFieldRenderer.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DIYShaderBlueprintFunctionLib.generated.h"

class FDIYPostOpaqueRenderer;
class FRHICommandListImmediate;
struct IPooledRenderTarget;

// Use To Fill FSimpleUniformStructParameters
USTRUCT(BlueprintType, meta = (ScriptName = "DIYShaderLibrary"))
struct FSimpleShaderParameter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	FLinearColor Color1 = FLinearColor(1.0, 0.0, 0.0, 1.0);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	FLinearColor Color2 = FLinearColor(0.0, 1.0, 0.0, 1.0);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	FLinearColor Color3 = FLinearColor(0.0, 0.0, 1.0, 1.0);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	FLinearColor Color4 = FLinearColor(1.0, 1.0, 1.0, 1.0);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	int32 ColorIndex = 1;
};



UCLASS(MinimalAPI, meta = (ScriptName = "DIYShaderLibrary"))
class UDIYShaderBlueprintFunctionLib : public  UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UDIYShaderBlueprintFunctionLib() {}
	UFUNCTION(BlueprintCallable, Category = "DIYShaders", meta = (WorldContext = "WorldContextObject"))
	static void DrawColorBoard(
		const UObject* WorldContextObject,
		UTextureRenderTarget2D* OutputRenderTarget,
		FSimpleShaderParameter Parameter,
		UTexture2D *InTexture,
		FLinearColor InColor);
	UFUNCTION(BlueprintCallable, Category = "DIYShaders", meta = (WorldContext = "WorldContextObject"))
	static void DrawColorCS(
		const UObject* WorldContextObject,
		UTextureRenderTarget2D *InTexture
	);
};



