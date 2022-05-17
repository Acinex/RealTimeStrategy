// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSComponentRegistry.h"

#include "RTSLog.h"

TSet<AActor*> URTSComponentRegistry::GetAllActors()
{
	TSet<AActor*> Actors;

	for (TWeakObjectPtr<UActorComponent> RegisteredComponent : RegisteredComponents)
	{
		if (RegisteredComponent.IsValid())
		{
			Actors.Add(RegisteredComponent->GetOwner());
		}
	}

	return Actors;
}

TSet<TWeakObjectPtr<UActorComponent>> URTSComponentRegistry::GetAllComponents()
{
	return RegisteredComponents;
}

template <class T>
TSet<TWeakObjectPtr<T>> URTSComponentRegistry::GetComponents()
{
	static_assert(TPointerIsConvertibleFromTo<T, const UActorComponent>::Value, "'T' template parameter to GetComponents must be derived from UActorComponent");

	TSet<TWeakObjectPtr<T>> Result;
	for (const TWeakObjectPtr<UActorComponent>& Component : RegisteredComponents)
	{
		if (Component.IsValid() && Component->GetClass()->IsChildOf(T::StaticClass()))
		{
			Result.Emplace(Cast<T>(Component));
		}
	}

	return Result;
}

void URTSComponentRegistry::Register(UActorComponent* Component)
{
	bool bIsAlreadyInSetPtr;
	RegisteredComponents.Add(Component, &bIsAlreadyInSetPtr);
	if (bIsAlreadyInSetPtr)
	{
		return;
	}

	UE_LOG(LogRTS, Warning, TEXT("Register: %s (%s)"), *Component->GetName(), *Component->GetOwner()->GetActorLabel())

	OnActorRegistered.Broadcast(Component);
}

void URTSComponentRegistry::Deregister(UActorComponent* Component)
{
	RegisteredComponents.Remove(Component);
	OnActorUnRegistered.Broadcast(Component);
}
