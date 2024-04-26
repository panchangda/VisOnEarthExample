// Fill out your copyright notice in the Description page of Project Settings.


#include "VisOnEarthUIWidget.h"
#include "FlowFieldUIWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"


void UVisOnEarthUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AddFlowFieldButton->OnClicked.AddDynamic(this, &UVisOnEarthUIWidget::OnAddFlowFieldButtonClicked);
}

void UVisOnEarthUIWidget::OnAddFlowFieldButtonClicked()
{
	// if (FlowFieldUIWidgets)
	if(false)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlowFieldActor UI Already Exists!"));
	}
	else
	{
		// Initialize UI Widget
		UClass* BP_FlowFieldUI = LoadClass<UFlowFieldUIWidget>(nullptr, TEXT("/VisOnEarth/BP_FlowFieldUI.BP_FlowFieldUI_C"));
		FlowFieldUIWidgets = CreateWidget<UFlowFieldUIWidget>(GetWorld(), BP_FlowFieldUI);
		
		UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(VisOptionScrollBox->AddChild(FlowFieldUIWidgets));
		ScrollBoxSlot->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f));
	}
}
