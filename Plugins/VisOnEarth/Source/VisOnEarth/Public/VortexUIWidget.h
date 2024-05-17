// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VortexUIWidget.generated.h"

class AVortexActor;
/**
 * 
 */
UCLASS()
class VISONEARTH_API UVortexUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex BPs")
	TSubclassOf<AVortexActor> VortexActorBlueprint;

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* DestroyButton;

	UFUNCTION()
	void OnDestroyButtonClicked();

	TWeakObjectPtr<AVortexActor> VortexActor;
};
