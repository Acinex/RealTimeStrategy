// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Abilities/RTSGEHealthRegen.h"
#include "Combat/RTSCombatComponent.h"


URTSGEHealthRegen::URTSGEHealthRegen(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void URTSGEHealthRegen::Update(const URTSCombatComponent* CombatComponent, float Change)
{
	const URTSAttributeSet* AttributeSet = CombatComponent->GetRTSAttributeSet();

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = AttributeSet->GetHealthAttribute();
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(Change);

	Modifiers.Add(ModifierInfo);
}
