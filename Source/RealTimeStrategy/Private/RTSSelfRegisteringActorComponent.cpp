// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSSelfRegisteringActorComponent.h"

#include "Kismet/GameplayStatics.h"


URTSSelfRegisteringActorComponent::URTSSelfRegisteringActorComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void URTSSelfRegisteringActorComponent::BeginPlay()
{
	Super::BeginPlay();
	ComponentRegistry = UGameplayStatics::GetGameInstance(this)->GetSubsystem<URTSComponentRegistry>();
	ComponentRegistry->Register(this);
}

void URTSSelfRegisteringActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (IsValid(ComponentRegistry))
	{
		ComponentRegistry->Deregister(this);
	}
}
