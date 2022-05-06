// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "RTSRegenerateHealthTask.generated.h"

/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSRegenerateHealthTask : public UGameplayTask
{
	GENERATED_BODY()
public:
	explicit URTSRegenerateHealthTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void TickTask(float DeltaTime) override;
};
