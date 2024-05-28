// Fill out your copyright notice in the Description page of Project Settings.


#include "VisOnEarthUIWidget.h"
#include "FlowFieldUIWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"

#include "EngineUtils.h"
#include "LICUIWidget.h"
#include "VortexActor.h"
#include "VortexUIWidget.h"


void UVisOnEarthUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AddFlowFieldButton->OnClicked.AddDynamic(this, &UVisOnEarthUIWidget::OnAddFlowFieldButtonClicked);
	AddIsoLineButton->OnClicked.AddDynamic(this, &UVisOnEarthUIWidget::OnAddIsoLineButtonClicked);
	AddLICButton->OnClicked.AddDynamic(this, &UVisOnEarthUIWidget::OnAddLICButtonClicked);
	AddVortexButton->OnClicked.AddDynamic(this, &UVisOnEarthUIWidget::OnAddVortexButtonClicked);
}

void UVisOnEarthUIWidget::OnAddFlowFieldButtonClicked()
{
	// if (FlowFieldUIWidgets)
	if (false)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlowFieldActor UI Already Exists!"));
	}
	else
	{
		// Initialize UI Widget
		UClass* BP_FlowFieldUI = LoadClass<UFlowFieldUIWidget>(
			nullptr, TEXT("/VisOnEarth/UI/BP_FlowFieldUI.BP_FlowFieldUI_C"));
		FlowFieldUIWidgets = CreateWidget<UFlowFieldUIWidget>(GetWorld(), BP_FlowFieldUI);

		UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(VisOptionScrollBox->AddChild(FlowFieldUIWidgets));
		ScrollBoxSlot->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f));
	}
}

void UVisOnEarthUIWidget::OnAddLICButtonClicked()
{
	// Initialize UI Widget
	UClass* BP_LICUI = LoadClass<ULICUIWidget>(
		nullptr, TEXT("/VisOnEarth/UI/BP_LICUI.BP_LICUI_C"));
	ULICUIWidget* LICUI = CreateWidget<ULICUIWidget>(GetWorld(), BP_LICUI);

	UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(VisOptionScrollBox->AddChild(LICUI));
	ScrollBoxSlot->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f));
}

void UVisOnEarthUIWidget::OnAddVortexButtonClicked()
{
	// Initialize UI Widget
	UClass* UIBPClass = LoadClass<UVortexUIWidget>(
		nullptr, TEXT("/VisOnEarth/UI/BP_VortexUI.BP_VortexUI_C"));
	UVortexUIWidget* UIWidget = CreateWidget<UVortexUIWidget>(GetWorld(), UIBPClass);

	UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(VisOptionScrollBox->AddChild(UIWidget));
	ScrollBoxSlot->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f));
}

void UVisOnEarthUIWidget::OnAddIsoLineButtonClicked()
{
	// UWorld* World = GetWorld();
	// if (World)
	// {
	// 	auto IsoLineActor = World->SpawnActor<AMCSActor>(AMCSActor::StaticClass());
	// 	IsoLineActor->GeoComponent->GeoRef = [&]()-> ACesiumGeoreference* {
	// 		UWorld* World = GetWorld();
	// 		if (!IsValid(World))
	// 			return nullptr;
	//
	// 		// Note: The actor iterator will be created with the
	// 		// "EActorIteratorFlags::SkipPendingKill" flag,
	// 		// meaning that we don't have to handle objects
	// 		// that have been deleted. (This is the default,
	// 		// but made explicit here)
	// 		ACesiumGeoreference* Georeference = nullptr;
	// 		EActorIteratorFlags flags = EActorIteratorFlags::OnlyActiveLevels |
	// 			EActorIteratorFlags::SkipPendingKill;
	// 		for (TActorIterator<AActor> actorIterator(
	// 			     World,
	// 			     ACesiumGeoreference::StaticClass(),
	// 			     flags);
	// 		     actorIterator;
	// 		     ++actorIterator)
	// 		{
	// 			AActor* actor = *actorIterator;
	// 			if (actor->GetLevel() == World->PersistentLevel &&
	// 				actor->ActorHasTag(FName("DEFAULT_GEOREFERENCE")))
	// 			{
	// 				Georeference = Cast<ACesiumGeoreference>(actor);
	// 				break;
	// 			}
	// 		}
	//
	// 		return Georeference;
	// 	}();
	// 	IsoLineActor->VolumeComponent->LoadRAWVolume();
	// 	IsoLineActor->VolumeComponent->LoadTF();
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("FlowFieldUIWidget UI Cannot Get World!"));
	// }
}
