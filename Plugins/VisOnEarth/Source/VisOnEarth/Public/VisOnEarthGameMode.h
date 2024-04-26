// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VisOnEarthGameMode.generated.h"


class UVisOnEarthUIWidget;
/**
 * 
 */
UCLASS()
class VISONEARTH_API AVisOnEarthGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	AVisOnEarthGameMode();

protected:
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaSeconds)override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

private:
	UVisOnEarthUIWidget* VisOnEarthUI;
};
