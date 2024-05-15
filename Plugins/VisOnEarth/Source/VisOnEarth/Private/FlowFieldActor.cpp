// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldActor.h"

#include "EngineUtils.h"
#include "FlowFieldUIWidget.h"
#include "Interfaces/IPluginManager.h"

void AFlowFieldActor::UpdateFlowFieldSettings(const FFlowFieldSettings& NewFlowFieldSettings)
{
	// Update Self Settings
	FlowFieldSettings = NewFlowFieldSettings;
	// Update Renderer Settings
	UpdateRendererSettings();
	
}


void AFlowFieldActor::UpdateRendererSettings()
{
	if(renderer.IsValid())
	{
		renderer->UpdateSettingsFromGame(FlowFieldSettings);
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
	}
}

void AFlowFieldActor::UpdateSettingsFromRenderer()
{
	if(renderer.IsValid())
	{
		FlowFieldSettings = renderer->GetSetttings();
	}else
	{
		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
	}
}
// Sets default values
AFlowFieldActor::AFlowFieldActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

static const FName DEFAULT_GEOREFERENCE_TAG = FName("DEFAULT_GEOREFERENCE");

// Called when the game starts or when spawned
void AFlowFieldActor::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("WindField Actor is initialized!") );

	renderer = MakeShared<FFlowFieldRenderer>();
	if(!renderer.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("PostOpaque Renderer cannot be initialized!") );
	}
	// find default georeference
	ACesiumGeoreference* DefaultGeoReference = FindTaggedGeoReference(DEFAULT_GEOREFERENCE_TAG);
	if(IsValid(DefaultGeoReference))
	{
		GeoReference = DefaultGeoReference;
		UpdateRendererGeoReference();
	}else
	{
		UE_LOG(LogActor, Error, TEXT("Flow Field Actor Can't Find Cesium Default GeoReference!"));
	}

	// Use Local FlowFieldSettings to Setup Renderer, Trigger Renderer Resource ReCreate, Update and so on
	UpdateRendererSettings();
	
	// Register Render() to PostOpaqueExtensions Delegate
	RegisterRenderer();
}
void AFlowFieldActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Warning, TEXT("Flow Field Actor is Destoyed!") );
	UnregisterRenderer();
}

void AFlowFieldActor::RegisterRenderer()
{
	if(!renderer.IsValid()) return;

	ENQUEUE_RENDER_COMMAND(RegisterRenderScreenRenderer)
	  ([renderer = this->renderer](FRHICommandListImmediate &RHICmdList) {
		  renderer->RegisterRendererDelegate();
	  });
}

void AFlowFieldActor::UnregisterRenderer()
{
	
	ENQUEUE_RENDER_COMMAND(UnregisterRenderScreenRenderer)
	  ([renderer = this->renderer](FRHICommandListImmediate &RHICmdList) {
		  renderer->UnregisterRendererDelegate();
	  });

	renderer.Reset();
}


ACesiumGeoreference* AFlowFieldActor::FindTaggedGeoReference(const FName& Tag)
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


// Called every frame
void AFlowFieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlowFieldActor::UpdateRendererGeoReference() const
{
	// Update Corresponding Render Thread Resources
	if(renderer.IsValid())
	{
		renderer->UpdateGeoReference(this->GeoReference);
	}else
	{
		UE_LOG(LogActor, Error, TEXT("Flow Field Actor's renderer is Invalid!"));
	}
}


#if WITH_EDITOR
void AFlowFieldActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AFlowFieldActor, GeoReference))
	{
		UpdateRendererGeoReference();
	}
	
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(AFlowFieldActor, FlowFieldSettings))
	{
		UpdateRendererSettings();
	}
}
#endif


