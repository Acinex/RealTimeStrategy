// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Combat/RTSCombatComponent.h"
#include "RTSGEConstruction.generated.h"

/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSGEConstruction : public UGameplayEffect
{
	GENERATED_BODY()
public:
	explicit URTSGEConstruction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Update(const URTSCombatComponent* CombatComponent, float Change);
	
};
