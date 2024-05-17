// Fill out your copyright notice in the Description page of Project Settings.


#include "LICUIWidget.h"

#include "LICActor.h"
#include "Components/Button.h"

void ULICUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UWorld* World = GetWorld();
	if(World)
	{
		if(LICActorBlueprint)
		{
			LICActor = World->SpawnActor<ALICActor>(LICActorBlueprint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LICActorBlueprint is not set!"));
		}
		
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("LIC UI Cannot Get World!"));
	}
	
	DestroyButton->OnClicked.AddDynamic(this, &ULICUIWidget::OnDestroyButtonClicked);
	SaveButton->OnClicked.AddDynamic(this, &ULICUIWidget::OnSaveButtonClicked);
	
}

void ULICUIWidget::OnDestroyButtonClicked()
{
	if (LICActor.IsValid())
	{
		LICActor->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UFlowFieldUIWidget Has No Valid FlowFieldActor"));
	}

	this->RemoveFromParent();
}

void ULICUIWidget::OnSaveButtonClicked()
{
	if(LICActor.IsValid())
	{
		LICActor->SaveFlowFieldTexture2DToAsset();
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("UFlowFieldUIWidget Has No Valid FlowFieldActor"));
	}
}
