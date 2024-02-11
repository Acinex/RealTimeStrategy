// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Abilities/AttackMoveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Orders/RTSAttackMoveOrder.h"

void UAttackMoveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(this, FName("WaitForConfirm"), EGameplayTargetingConfirmation::UserConfirmed, AGameplayAbilityTargetActor_GroundTrace::StaticClass());

	Task->ValidData.AddDynamic(this, &UAttackMoveAbility::Confirm);
}

void UAttackMoveAbility::Confirm(const FGameplayAbilityTargetDataHandle& Data)
{
	FHitResult HitResultFromTargetData = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 0);

	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

	FRTSOrderTargetData OrderTargetData;
	OrderTargetData.Actor = HitResultFromTargetData.GetActor();
	OrderTargetData.Location = HitResultFromTargetData.Location;

	URTSAttackMoveOrder::StaticClass()->GetDefaultObject<URTSOrder>()->SetOrder(Cast<APawn>(CurrentActorInfo->OwnerActor), OrderTargetData, 0);
}
