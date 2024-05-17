// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisOnEarthUIWidget.generated.h"

class ULICUIWidget;
class UFlowFieldUIWidget;
class AFlowFieldActor;
/**
 * 
 */
UCLASS()
class VISONEARTH_API UVisOnEarthUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MainCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddFlowFieldButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddLICButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* AddDirectVolumeRenderingButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* AddIsoSurfaceButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddIsoLineButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddVortexButton;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* VisOptionScrollBox;

	UFUNCTION()
	void OnAddFlowFieldButtonClicked();

	UFUNCTION()
	void OnAddIsoLineButtonClicked();

	UFUNCTION()
	void OnAddLICButtonClicked();
	
	UFUNCTION()
	void OnAddVortexButtonClicked();
	
	UFlowFieldUIWidget* FlowFieldUIWidgets;
};
