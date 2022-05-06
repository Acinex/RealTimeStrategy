// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RTSGEHealthRegen.generated.h"

class URTSCombatComponent;
/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSGEHealthRegen : public UGameplayEffect
{
	GENERATED_BODY()
public:
	explicit URTSGEHealthRegen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void Update(const URTSCombatComponent* CombatComponent, float Change);
};
