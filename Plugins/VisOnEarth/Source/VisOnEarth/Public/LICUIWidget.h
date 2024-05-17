// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LICUIWidget.generated.h"

class ALICActor;
/**
 * 
 */
UCLASS()
class VISONEARTH_API ULICUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LIC BPs")
	TSubclassOf<ALICActor> LICActorBlueprint;
private:

	UPROPERTY(meta = (BindWidget))
	class UButton* DestroyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SaveButton;
	

	UFUNCTION()
	void OnDestroyButtonClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	TWeakObjectPtr<ALICActor> LICActor;
};
