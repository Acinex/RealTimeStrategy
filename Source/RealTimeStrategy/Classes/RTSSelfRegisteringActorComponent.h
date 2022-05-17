// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSComponentRegistry.h"
#include "Components/ActorComponent.h"
#include "RTSSelfRegisteringActorComponent.generated.h"


UCLASS(ClassGroup="RTS", Category="RTS", Abstract)
class REALTIMESTRATEGY_API URTSSelfRegisteringActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit URTSSelfRegisteringActorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	URTSComponentRegistry* ComponentRegistry;

};
