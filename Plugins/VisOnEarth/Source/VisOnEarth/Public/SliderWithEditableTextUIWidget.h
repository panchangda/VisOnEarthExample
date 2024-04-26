// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderWithEditableTextUIWidget.generated.h"

class UTextBlock;
class USlider;
class UEditableText;
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FSliderWithEditableTextValueChangedDelegate, float);

UENUM(BlueprintType)
enum class UIVariableType : uint8
{
	Int,
	Float
};


UCLASS()
class VISONEARTH_API USliderWithEditableTextUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FSliderWithEditableTextValueChangedDelegate OnValueChangedDelegate;
	

	UFUNCTION()
	void SetValueRange(float MinValue, float MaxValue);

	UFUNCTION()
	void SetValue(float Value);

	UFUNCTION()
	void SetTextBlock(const FText& Text);

	UFUNCTION()
	void InitializeBasic(const FText& Text, float Value, float MinValue, float MaxValue, UIVariableType VarType);

	UFUNCTION()
	void SetUIVariableType(const UIVariableType& InType);
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MyTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	USlider* MySlider;

	UPROPERTY(meta = (BindWidget))
	UEditableText* MyEditableText;
	
	UFUNCTION()
	void OnSliderValueChanged(float Value);

	UFUNCTION()
	void OnTextValueChanged(const FText& Text);

	UIVariableType VarType = UIVariableType::Float;
};
