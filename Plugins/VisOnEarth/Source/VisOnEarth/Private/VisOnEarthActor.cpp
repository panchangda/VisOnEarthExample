// Fill out your copyright notice in the Description page of Project Settings.


#include "VisOnEarthActor.h"
#include "CesiumGeoreference.h"
#include "EngineUtils.h"
#include "VisOnEarthRenderer.h"


// Sets default values
AVisOnEarthActor::AVisOnEarthActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVisOnEarthActor::BeginPlay()
{
	Super::BeginPlay();
	// Get Default GeoReference
	DefaultGeoReference = FindTaggedGeoReference(DefaultGeoReferenceTag);
	if(!IsValid(DefaultGeoReference))
	{
		UE_LOG(LogActor, Error, TEXT("VisOnEarth Actor Can't Find Cesium Default GeoReference!"));
	}
	
	// Initialize Renderer
	if(IsValid(RendererClass))
	{
		Renderer = NewObject<UVisOnEarthRenderer>(RendererClass);
		RegisterRenderer();
	}
}

void AVisOnEarthActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnregisterRenderer();
}

// Called every frame
void AVisOnEarthActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisOnEarthActor::RegisterRenderer()
{
	// Renderer already exists?
	if(IsValid(Renderer)) return;
	
	// Renderer is base class: do not register
	if(RendererClass == UVisOnEarthRenderer::StaticClass()) return;

	ENQUEUE_RENDER_COMMAND(RegisterRenderScreenRenderer)
	  ([renderer = this->Renderer](FRHICommandListImmediate &RHICmdList) {
		  renderer->RegisterRendererDelegate();
	  });
}

void AVisOnEarthActor::UnregisterRenderer()
{
	if(!IsValid(Renderer)) return;
	
	ENQUEUE_RENDER_COMMAND(UnregisterRenderScreenRenderer)
	  ([renderer = this->Renderer](FRHICommandListImmediate &RHICmdList) {
		  renderer->UnregisterRendererDelegate();
	  });

	Renderer = nullptr;
}


ACesiumGeoreference* AVisOnEarthActor::FindTaggedGeoReference(const FName& Tag)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
		return nullptr;

	// Note: The actor iterator will be created with the
	// "EActorIteratorFlags::SkipPendingKill" flag,
	// meaning that we don't have to handle objects
	// that have been deleted. (This is the default,
	// but made explicit here)
	ACesiumGeoreference* Georeference = nullptr;
	EActorIteratorFlags flags = EActorIteratorFlags::OnlyActiveLevels |
								EActorIteratorFlags::SkipPendingKill;
	for (TActorIterator<AActor> actorIterator(
			 World,
			 ACesiumGeoreference::StaticClass(),
			 flags);
		 actorIterator;
		 ++actorIterator) {
		AActor* actor = *actorIterator;
		if (actor->GetLevel() == World->PersistentLevel &&
			actor->ActorHasTag(Tag)) {
			Georeference = Cast<ACesiumGeoreference>(actor);
			break;
			}
		 }

	return Georeference;
}