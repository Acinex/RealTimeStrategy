﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Abilities/RTSRangedAttackAbility.h"

#include "AIController.h"
#include "RTSLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Combat/RTSAbilityProjectile.h"
#include "Combat/RTSCombatComponent.h"
#include "Libraries/RTSGameplayTagLibrary.h"

URTSRangedAttackAbility::URTSRangedAttackAbility(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	ActivationBlockedTags.AddTag(URTSGameplayTagLibrary::Attack_Cooldown());
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::Type::ReplicateYes;
}

void URTSRangedAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ProjectileClass.Get())
	{
		UE_LOG(LogRTS, Error, TEXT("URTSRangedAttackAbility: No Projectile Class"));
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo, nullptr);

	// Fire projectile.
	// Build spawn transform.
	const FVector SpawnLocation = ActorInfo->OwnerActor->GetActorLocation();
	const FRotator SpawnRotation = ActorInfo->OwnerActor->GetActorRotation();
	const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

	// Build spawn info.
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Cast<APawn>(ActorInfo->OwnerActor);
	SpawnInfo.Owner = Cast<APawn>(ActorInfo->OwnerActor);
	SpawnInfo.ObjectFlags |= RF_Transient;

	AAIController* Controller = Cast<AAIController>(Cast<APawn>(ActorInfo->OwnerActor)->GetController());

	if (!IsValid(Controller))
	{
		return;
	}

	const AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!IsValid(Target))
	{
		return;
	}

	// Spawn projectile.

	if (ARTSAbilityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ARTSAbilityProjectile>(ProjectileClass, SpawnTransform, SpawnInfo))
	{
		UE_LOG(LogRTS, Log, TEXT("%s fired projectile %s at target %s."), *ActorInfo->OwnerActor->GetName(), *SpawnedProjectile->GetName(), *Target->GetName());

		// Aim at target.
		SpawnedProjectile->SetTarget(Target);

		const URTSCombatComponent* CombatComponent = Cast<URTSCombatComponent>(ActorInfo->AbilitySystemComponent.Get());
		CombatComponent->OnAttackUsed.Broadcast(ActorInfo->OwnerActor.Get(), Target, SpawnedProjectile);
	}
	else
	{
		UE_LOG(LogRTS, Warning, TEXT("No Projectile was spawned"));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
