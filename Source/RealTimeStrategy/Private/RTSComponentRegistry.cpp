// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSComponentRegistry.h"

#include "RTSLog.h"

TSet<AActor*> URTSComponentRegistry::GetAllActors()
{
	TSet<AActor*> Actors;

	for (TWeakObjectPtr<UActorComponent> RegisteredComponent : GetAllComponents())
	{
		if (RegisteredComponent.IsValid())
		{
			Actors.Add(RegisteredComponent->GetOwner());
		}
	}

	return Actors;
}

TArray<TWeakObjectPtr<UActorComponent>> URTSComponentRegistry::GetAllComponents()
{
	TArray<TWeakObjectPtr<UActorComponent>> Components;
	RegisteredComponents.GenerateValueArray(Components);
	return Components;
}

template <class T>
TSet<TWeakObjectPtr<T>> URTSComponentRegistry::GetComponents()
{
	static_assert(TPointerIsConvertibleFromTo<T, const UActorComponent>::Value, "'T' template parameter to GetComponents must be derived from UActorComponent");

	TArray<TWeakObjectPtr<UActorComponent>> Components;
	RegisteredComponents.MultiFind(T::StaticClass(), Components);

	TSet<TWeakObjectPtr<T>> Result;
	for (const TWeakObjectPtr<UActorComponent>& Component : Components)
	{
		if (Component.IsValid())
		{
			Result.Emplace(Cast<T>(Component.Get()));
		}
	}

	return Result;
}

void URTSComponentRegistry::Register(UActorComponent* Component)
{
	RegisteredComponents.Add(Component->GetClass(), Component);

	UE_LOG(LogRTS, Log, TEXT("Register: %s (%s)"), *Component->GetName(), *Component->GetOwner()->GetActorLabel())

	OnActorRegistered.Broadcast(Component);
}

void URTSComponentRegistry::Deregister(UActorComponent* Component)
{
	RegisteredComponents.Remove(Component->GetClass(), Component);
	OnActorUnRegistered.Broadcast(Component);
}
