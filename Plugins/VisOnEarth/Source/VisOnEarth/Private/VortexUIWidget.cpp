// Fill out your copyright notice in the Description page of Project Settings.


#include "VortexUIWidget.h"

#include "LICUIWidget.h"
#include "VortexActor.h"
#include "Components/Button.h"

void UVortexUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UWorld* World = GetWorld();
	if(World)
	{
		if(VortexActorBlueprint)
		{
			VortexActor = World->SpawnActor<AVortexActor>(VortexActorBlueprint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("VortexActorBlueprint is not set!"));
		}
		
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Vortex UI Cannot Get World!"));
	}
	
	DestroyButton->OnClicked.AddDynamic(this, &UVortexUIWidget::OnDestroyButtonClicked);

	
}

void UVortexUIWidget::OnDestroyButtonClicked()
{
	if (VortexActor.IsValid())
	{
		VortexActor->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VortexUI Has No Valid VortexActor"));
	}

	this->RemoveFromParent();
}