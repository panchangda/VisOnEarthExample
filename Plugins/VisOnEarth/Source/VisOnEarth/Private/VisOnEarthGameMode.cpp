// Fill out your copyright notice in the Description page of Project Settings.


#include "VisOnEarthGameMode.h"

#include "Cesium3DTileset.h"
#include "CesiumIonRasterOverlay.h"
#include "CesiumTileMapServiceRasterOverlay.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "VisOnEarthUIWidget.h"
#include "Kismet/GameplayStatics.h"


AVisOnEarthGameMode::AVisOnEarthGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVisOnEarthGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get CommandLine
	const TCHAR* CommandLine = FCommandLine::Get();

	// Set terrain's msse/mesh url/texture url
	int TerrainMSSE = 0;
	if (FParse::Value(CommandLine, TEXT("-terrain_msse"), TerrainMSSE))
	{
		Terrain->SetMaximumScreenSpaceError(TerrainMSSE);
	}

	FString TerrainMeshUrl;
	if (FParse::Value(CommandLine, TEXT("-terrain_mesh_url"), TerrainMeshUrl))
	{
		Terrain->SetTilesetSource(ETilesetSource::FromUrl);
		Terrain->SetUrl(TerrainMeshUrl);
	}

	FString TerrainTextureUrl;
	if (FParse::Value(CommandLine, TEXT("-terrain_texture_url"), TerrainTextureUrl))
	{
		UCesiumTileMapServiceRasterOverlay* TMSRasterOverlayComponent = Cast<UCesiumTileMapServiceRasterOverlay>(
			Terrain->GetComponentByClass(UCesiumTileMapServiceRasterOverlay::StaticClass()));

		if (TMSRasterOverlayComponent)
		{
			TMSRasterOverlayComponent->Url = TerrainTextureUrl;
			TMSRasterOverlayComponent->Refresh();
		}
		else
		{
			UCesiumIonRasterOverlay* IonRasterOverlayComponent = Cast<UCesiumIonRasterOverlay>(
				Terrain->GetComponentByClass(UCesiumIonRasterOverlay::StaticClass()));
			if (IonRasterOverlayComponent)
			{
				IonRasterOverlayComponent->Deactivate();
				IonRasterOverlayComponent->RemoveFromTileset();
				IonRasterOverlayComponent->DestroyComponent();
			}

			UCesiumTileMapServiceRasterOverlay* RuntimeTMSRasterOverlayComponent = NewObject<UCesiumTileMapServiceRasterOverlay>(
				Terrain.Get(), UCesiumTileMapServiceRasterOverlay::StaticClass());
			if (RuntimeTMSRasterOverlayComponent)
			{
				RuntimeTMSRasterOverlayComponent->RegisterComponent();
				RuntimeTMSRasterOverlayComponent->AddToTileset();
				RuntimeTMSRasterOverlayComponent->Url = TerrainTextureUrl;
				RuntimeTMSRasterOverlayComponent->Refresh();
			}
			else
			{
				UE_LOG(LogClass, Error, TEXT("Failed to Create TMSRasterOverlayComponent For Offline Data Url at Runtime!"));
			}
		}
	}


	// Show Mouse Cursor
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController);
	PlayerController->bShowMouseCursor = true;


	// Create VisOnEarth UI
	UClass* BP_VisOnEarthUI = LoadClass<UVisOnEarthUIWidget>(nullptr, TEXT("/VisOnEarth/BP_VisOnEarthUI.BP_VisOnEarthUI_C"));
	VisOnEarthUI = CreateWidget<UVisOnEarthUIWidget>(GetWorld(), BP_VisOnEarthUI);
	VisOnEarthUI->AddToViewport(0);
	
}

void AVisOnEarthGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AVisOnEarthGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}