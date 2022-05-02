#include "Construction/RTSGEConstruction.h"

URTSGEConstruction::URTSGEConstruction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void URTSGEConstruction::Update(const URTSCombatComponent* CombatComponent, float Change)
{
	const URTSAttributeSet* AttributeSet = CombatComponent->GetRTSAttributeSet();

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = AttributeSet->GetHealthAttribute();
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(Change);

	Modifiers.Add(ModifierInfo);
}
