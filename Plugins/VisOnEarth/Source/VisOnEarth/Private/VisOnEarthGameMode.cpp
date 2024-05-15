// Fill out your copyright notice in the Description page of Project Settings.


#include "VisOnEarthGameMode.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "VisOnEarthUIWidget.h"
#include "Kismet/GameplayStatics.h"


AVisOnEarthGameMode::AVisOnEarthGameMode()
{
	// PrimaryActorTick.bCanEverTick = false;
}

void AVisOnEarthGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController);
	PlayerController->bShowMouseCursor = true;
	
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