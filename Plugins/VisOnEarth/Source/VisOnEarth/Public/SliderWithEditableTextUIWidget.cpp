// Fill out your copyright notice in the Description page of Project Settings.


#include "SliderWithEditableTextUIWidget.h"

#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"


void USliderWithEditableTextUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind events
	if (MySlider)
	{
		MySlider->OnValueChanged.AddDynamic(this, &USliderWithEditableTextUIWidget::OnSliderValueChanged);
	}
	if (MyEditableText)
	{
		MyEditableText->OnTextChanged.AddDynamic(this, &USliderWithEditableTextUIWidget::OnTextValueChanged);
	}
}


void USliderWithEditableTextUIWidget::OnSliderValueChanged(float Value)
{
	if(MyEditableText)
	{
		
		if(VarType == UIVariableType::Float)
		{
			MyEditableText->SetText(FText::AsNumber(Value));
		}
		else if(VarType == UIVariableType::Int)
		{
			MyEditableText->SetText(FText::AsNumber(FMath::RoundToInt(Value)));
		}

	}
	if(OnValueChangedDelegate.IsBound())
	{
		OnValueChangedDelegate.Execute(Value);
	}
}

void USliderWithEditableTextUIWidget::OnTextValueChanged(const FText& Text)
{
	float ValueFromFText = FCString::Atof(*Text.ToString());
	if(VarType == UIVariableType::Int)
	{
		ValueFromFText = FMath::RoundToInt(ValueFromFText);
	}

	if(MySlider)
	{
		MySlider->SetValue(ValueFromFText);
		if(ValueFromFText > MySlider->GetMaxValue())
		{
			MySlider->SetMaxValue(ValueFromFText);
		}
	}

	if(OnValueChangedDelegate.IsBound())
	{
		OnValueChangedDelegate.Execute(ValueFromFText);
	}
}

void USliderWithEditableTextUIWidget::SetTextBlock(const FText& Text)
{
	MyTextBlock->SetText(Text);
}

void USliderWithEditableTextUIWidget::SetValue(float Value)
{
	MySlider->SetValue(Value);
	MyEditableText->SetText(FText::AsNumber(Value));
}

void USliderWithEditableTextUIWidget::SetValueRange(float MinValue, float MaxValue)
{
	MySlider->SetMinValue(MinValue);
	MySlider->SetMaxValue(MaxValue);
}

void USliderWithEditableTextUIWidget::InitializeBasic(const FText& Text, float Value, float MinValue, float MaxValue, UIVariableType Type)
{
	SetTextBlock(Text);
	SetValue(Value);
	SetValueRange(MinValue, MaxValue);
	SetUIVariableType(Type);
}

void USliderWithEditableTextUIWidget::SetUIVariableType(const UIVariableType& Type)
{
	VarType = Type;
}



