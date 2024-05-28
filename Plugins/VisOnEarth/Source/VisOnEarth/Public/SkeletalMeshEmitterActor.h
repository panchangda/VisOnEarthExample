// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SkeletalMeshEmitterActor.generated.h"

UCLASS()
class VISONEARTH_API ASkeletalMeshEmitterActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletalMeshEmitterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	TSubclassOf<USkeletalMeshComponent> EmitterSkeletalMeshClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	USkeletalMeshComponent* EmitterSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	float TimeInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	float LifeSpan = 5.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	FVector StartPosition =  FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Components")
	FVector Velocity =  FVector::ZeroVector;
	
	UFUNCTION()
	void SpawnSkeletalMesh();

	UFUNCTION()
	void DestroySkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent);

private:
	FTimerHandle TimerHandle;
};
