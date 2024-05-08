// Fill out your copyright notice in the Description page of Project Settings.


#include "MCSUIWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"

void UMCSUIWidget::NativeConstruct() {
    Super::NativeConstruct();

    AddToViewport(0);

    LoadRAWVolumeButton->OnClicked.AddDynamic(this, &UMCSUIWidget::OnLoadRAWVolumeButtonClicked);
    LoadTFButton->OnClicked.AddDynamic(this, &UMCSUIWidget::OnLoadTFButtonClicked);
    SmoothComboBoxString->OnSelectionChanged.AddDynamic(
        this, &UMCSUIWidget::OnSmoothComboBoxStringSelectionChanged);
    InterpComboBoxString->OnSelectionChanged.AddDynamic(
        this, &UMCSUIWidget::OnInterpComboBoxStringSelectionChanged);
    LineTypeComboBoxString->OnSelectionChanged.AddDynamic(
        this, &UMCSUIWidget::OnLineTypeComboBoxStringSelectionChanged);
    HeightSlider->OnValueChanged.AddDynamic(
        this, &UMCSUIWidget::OnHeightSliderValueChanged);
    
}
void UMCSUIWidget::SetMCSActor(AMCSActor *InActor) {
    MCSActor = InActor;
}

void UMCSUIWidget::OnLoadTFButtonClicked() {
    if(MCSActor.IsValid()){
        MCSActor->VolumeComponent->LoadTF();
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
}

void UMCSUIWidget::OnLoadRAWVolumeButtonClicked() {
    if(MCSActor.IsValid()){
        MCSActor->VolumeComponent->LoadRAWVolume();
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
}

void UMCSUIWidget::OnInterpComboBoxStringSelectionChanged(FString SelectedItem,
                                                          ESelectInfo::Type SelectionType) {

    uint32 Selected = InterpComboBoxString->GetSelectedIndex();

    if(MCSActor.IsValid()){
        if(Selected == 0 && MCSActor->UseLerp == true){
            MCSActor->UseLerp = false;
            MCSActor->setupRenderer(true);
        }else if(Selected == 1 && MCSActor->UseLerp == false){
            MCSActor->UseLerp = true;
            MCSActor->setupRenderer(true);
        }
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
}


void UMCSUIWidget::OnSmoothComboBoxStringSelectionChanged(FString SelectedItem,
                                                          ESelectInfo::Type SelectionType) {
    uint32 Selected = SmoothComboBoxString->GetSelectedIndex();
    if(MCSActor.IsValid()){
        if(Selected == 0 && MCSActor->UseSmoothedVolume == true){
            MCSActor->UseSmoothedVolume = false;
            MCSActor->setupRenderer(true);
        }else if(Selected == 1 && MCSActor->UseSmoothedVolume == false){
            MCSActor->UseSmoothedVolume = true;
            MCSActor->setupRenderer(true);
        }
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
    
}

void UMCSUIWidget::OnLineTypeComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {

    uint32 Selected = LineTypeComboBoxString->GetSelectedIndex();
    if(MCSActor.IsValid()){
        if(Selected == 0 && MCSActor->LineStyle == EMCSLineStyle::Dash){
            MCSActor->LineStyle = EMCSLineStyle::Solid;
            MCSActor->setupRenderer(true);
        }else if(Selected == 1 && MCSActor->LineStyle == EMCSLineStyle::Solid){
            MCSActor->LineStyle = EMCSLineStyle::Dash;
            MCSActor->setupRenderer(true);
        }
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
}

void UMCSUIWidget::OnHeightSliderValueChanged(float NewValue) {
    if(MCSActor.IsValid()){
        MCSActor->HeightRange.Y = NewValue;
        MCSActor->setupRenderer(true);
    }else {
        UE_LOG(LogTemp, Warning, TEXT("UMCSUIWidget: MCSActor is Invalid!"));
    }
}
