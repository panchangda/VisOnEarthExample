// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalMeshEmitterActor.h"


// Sets default values
ASkeletalMeshEmitterActor::ASkeletalMeshEmitterActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EmitterSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EmitterSkeletalMesh"));
	RootComponent = EmitterSkeletalMesh;
	
}

// Called when the game starts or when spawned
void ASkeletalMeshEmitterActor::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASkeletalMeshEmitterActor::SpawnSkeletalMesh, TimeInterval, true);
}

// Called every frame
void ASkeletalMeshEmitterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	EmitterSkeletalMesh->SetWorldLocation(EmitterSkeletalMesh->GetComponentLocation()+FVector(0.0, 0.0, 1.0));
}

void ASkeletalMeshEmitterActor::SpawnSkeletalMesh()
{
	USkeletalMeshComponent* NewSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, EmitterSkeletalMeshClass);
	NewSkeletalMeshComponent->RegisterComponent();
	NewSkeletalMeshComponent->SetWorldLocation(StartPosition);
	// NewSkeletalMeshComponent->SetSimulatePhysics(true);
	// NewSkeletalMeshComponent->SetPhysicsLinearVelocity(Velocity);
	// NewSkeletalMeshComponent->AddForce(Velocity);
	NewSkeletalMeshComponent->AddImpulse(FVector(0.0, 0.0, 10.0)* 1000.0f);  // 添加前向冲力

	// Attach the new component to the root component
	NewSkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Schedule the destruction of the new skeletal mesh component
	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, FTimerDelegate::CreateUObject(this, &ASkeletalMeshEmitterActor::DestroySkeletalMesh, NewSkeletalMeshComponent), LifeSpan, false);
}

void ASkeletalMeshEmitterActor::DestroySkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->DestroyComponent();
	}
}