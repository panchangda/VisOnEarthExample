// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlowFieldActor.h"
#include "FlowFieldSettings.h"
#include "FlowFieldUIWidget.generated.h"


class USliderWithEditableTextUIWidget;

/**
 * 
 */
UCLASS()
class VISONEARTH_API UFlowFieldUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Flow Field UI Widget")
	void ConnectToActor(AFlowFieldActor* InActor);
	
	void SetFlowFieldSetting(FFlowFieldSettings InSettings);
	FFlowFieldSettings FlowFieldSettings;

	
protected:
	virtual void NativeConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* LineWidthComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* TrailLengthComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* ParticleSumComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* SpeedScaleFactorComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* TrailFadeOpacityComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* DropRateComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* HueRangeComponent;

	UPROPERTY(meta = (BindWidget))
	USliderWithEditableTextUIWidget* HueOffsetComponent;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* DestroyButton;
	



	UFUNCTION()
	void OnLineWidthComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnTrailLengthComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnParticleSumComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnSpeedScaleFactorComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnTrailFadeOpacityComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnDropRateComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnHueRangeComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnHueOffsetComponentValueChanged(float NewValue);

	UFUNCTION()
	void OnDestroyButtonClicked();
	
	UFUNCTION()
	void SetupActor();
	
	TWeakObjectPtr<AFlowFieldActor> FlowFieldActor;
};
