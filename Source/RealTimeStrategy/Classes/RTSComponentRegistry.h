// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSComponentRegistry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSActorRegistryComponentRegisterSignature, UActorComponent*, Component);

/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSComponentRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	TSet<AActor*> GetAllActors();

	TArray<TWeakObjectPtr<UActorComponent>> GetAllComponents();

	template <class T>
	TSet<TWeakObjectPtr<T>> GetComponents();

	void Register(UActorComponent* Component);
	void Deregister(UActorComponent* Component);

	FRTSActorRegistryComponentRegisterSignature OnActorRegistered;
	FRTSActorRegistryComponentRegisterSignature OnActorUnRegistered;

private:
	TMultiMap<UClass*,TWeakObjectPtr<UActorComponent>> RegisteredComponents;
};