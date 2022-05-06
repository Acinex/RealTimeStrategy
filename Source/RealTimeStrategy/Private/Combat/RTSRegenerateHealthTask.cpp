// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSRegenerateHealthTask.h"

#include "Combat/RTSCombatComponent.h"
#include "Combat/Abilities/RTSGEHealthRegen.h"

URTSRegenerateHealthTask::URTSRegenerateHealthTask(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	bTickingTask = true;
}

void URTSRegenerateHealthTask::TickTask(float DeltaTime)
{
	URTSCombatComponent* CombatComponent = Cast<URTSCombatComponent>(GetTaskOwner());

	URTSGEHealthRegen* Effect = NewObject<URTSGEHealthRegen>();
	Effect->Update(CombatComponent, CombatComponent->GetHealthRegenerationRate() * DeltaTime);

	const FGameplayEffectContextHandle Handle;
	CombatComponent->ApplyGameplayEffectToSelf(Effect, 1, Handle);
}
