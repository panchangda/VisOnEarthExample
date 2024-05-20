// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldUIWidget.h"

#include "SliderWithEditableTextUIWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"


void UFlowFieldUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UWorld* World = GetWorld();
	if(World)
	{
			FlowFieldActor = World->SpawnActor<AFlowFieldActor>(AFlowFieldActor::StaticClass());
			FlowFieldSettings = FlowFieldActor->FlowFieldSettings;
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("FlowFieldUIWidget UI Cannot Get World!"));
	}
	
	
	
	LineWidthComponent->InitializeBasic(FText::FromString("LineWidth"), FlowFieldSettings.LineWidth, 1.0f, 20.0f, UIVariableType::Int);
	LineWidthComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnLineWidthComponentValueChanged);
	
	TrailLengthComponent->InitializeBasic(FText::FromString("TrailLength"), FlowFieldSettings.TrailLength, 1.0f, 50.0f,  UIVariableType::Int);
	TrailLengthComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnTrailLengthComponentValueChanged);
	
	ParticleSumComponent->InitializeBasic(FText::FromString("ParticlesSum"), FlowFieldSettings.ParticlesSum, 1.0f, 100000.0f, UIVariableType::Int);
	ParticleSumComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnParticleSumComponentValueChanged);

	SpeedScaleFactorComponent->InitializeBasic(FText::FromString("SpeedScaleFactor"), FlowFieldSettings.SpeedScaleFactor, 0.01f, 0.5f,  UIVariableType::Float);
	SpeedScaleFactorComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnSpeedScaleFactorComponentValueChanged);
	
	TrailFadeOpacityComponent->InitializeBasic(FText::FromString("TrailFadeOpacity"), FlowFieldSettings.TrailFadeOpacity, 0.8f, 1.0f,  UIVariableType::Float);
	TrailFadeOpacityComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnTrailFadeOpacityComponentValueChanged);

	DropRateComponent->InitializeBasic(FText::FromString("DropRate"), FlowFieldSettings.DropRate, 0.01f, 1.0f,  UIVariableType::Float);
	DropRateComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnDropRateComponentValueChanged);
	
	HueRangeComponent->InitializeBasic(FText::FromString("HueRange"), FlowFieldSettings.HueRange, 0.0f, 360.0f,UIVariableType::Float);
	HueRangeComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnHueRangeComponentValueChanged);

	HueOffsetComponent->InitializeBasic(FText::FromString("HueOffset"), FlowFieldSettings.HueOffset, 0.0f, 360.0f,UIVariableType::Float);
	HueOffsetComponent->OnValueChangedDelegate.BindUObject(this, &UFlowFieldUIWidget::OnHueOffsetComponentValueChanged);

	ArrowButton->OnClicked.AddDynamic(this, &UFlowFieldUIWidget::OnArrowButtonClicked);

	SuspendButton->OnClicked.AddDynamic(this, &UFlowFieldUIWidget::OnSuspendButtonClicked);
	
	DestroyButton->OnClicked.AddDynamic(this, &UFlowFieldUIWidget::OnDestroyButtonClicked);

}

void UFlowFieldUIWidget::OnLineWidthComponentValueChanged(float NewValue)
{
	FlowFieldSettings.LineWidth = StaticCast<int32>(NewValue);
	SetupActor();
}
void UFlowFieldUIWidget::OnTrailLengthComponentValueChanged(float NewValue)
{
	FlowFieldSettings.TrailLength = StaticCast<int32>(NewValue);
	SetupActor();
}
void UFlowFieldUIWidget::OnParticleSumComponentValueChanged(float NewValue)
{
	FlowFieldSettings.ParticlesSum = StaticCast<int32>(NewValue);
	SetupActor();
}
void UFlowFieldUIWidget::OnSpeedScaleFactorComponentValueChanged(float NewValue)
{
	FlowFieldSettings.SpeedScaleFactor = NewValue;
	SetupActor();
}

void UFlowFieldUIWidget::OnHueOffsetComponentValueChanged(float NewValue)
{
	FlowFieldSettings.HueOffset = NewValue;
	SetupActor();
}
void UFlowFieldUIWidget::OnHueRangeComponentValueChanged(float NewValue)
{
	FlowFieldSettings.HueRange = NewValue;
	SetupActor();
}

void UFlowFieldUIWidget::OnTrailFadeOpacityComponentValueChanged(float NewValue)
{
	FlowFieldSettings.TrailFadeOpacity = NewValue;
	SetupActor();
}

void UFlowFieldUIWidget::OnDropRateComponentValueChanged(float NewValue)
{
	FlowFieldSettings.DropRate = NewValue;
	SetupActor();
}


void UFlowFieldUIWidget::SetupActor()
{
	if (FlowFieldActor.IsValid())
	{
		FlowFieldActor.Get()->UpdateFlowFieldSettings(FlowFieldSettings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FlowFieldUIWidget Has No Valid FlowFieldActor"));
	}
}

void UFlowFieldUIWidget::ConnectToActor(AFlowFieldActor* InActor)
{
	FlowFieldActor = InActor;
	SetFlowFieldSetting(InActor->FlowFieldSettings);
}

void UFlowFieldUIWidget::SetFlowFieldSetting(FFlowFieldSettings InSettings)
{
	FlowFieldSettings = InSettings;
}



void UFlowFieldUIWidget::OnDestroyButtonClicked()
{
	if (FlowFieldActor.IsValid())
	{
		FlowFieldActor->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UFlowFieldUIWidget Has No Valid FlowFieldActor"));
	}

	this->RemoveFromParent();
	
}

void UFlowFieldUIWidget::OnArrowButtonClicked()
{
	FlowFieldSettings.DrawArrow = !FlowFieldSettings.DrawArrow;
	SetupActor();
}

void UFlowFieldUIWidget::OnSuspendButtonClicked()
{
	FlowFieldSettings.Suspend = !FlowFieldSettings.Suspend;
	SetupActor();
}

