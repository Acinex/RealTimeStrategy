#include "UI/RTSActorWidgetComponent.h"

#include "RTSComponentRegistry.h"
#include "Kismet/GameplayStatics.h"


URTSActorWidgetComponent::URTSActorWidgetComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	// Set reasonable default values.
	Space = EWidgetSpace::Screen;
}

void URTSActorWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	ComponentRegistry = UGameplayStatics::GetGameInstance(this)->GetSubsystem<URTSComponentRegistry>();
	ComponentRegistry->Register(this);
}

void URTSActorWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ComponentRegistry->Deregister(this);
}
