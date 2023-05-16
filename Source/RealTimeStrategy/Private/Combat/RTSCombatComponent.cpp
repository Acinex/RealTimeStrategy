#include "Combat/RTSCombatComponent.h"
#include "RTSGameMode.h"
#include "RTSLog.h"
#include "RTSPlayerAdvantageComponent.h"
#include "RTSPlayerState.h"
#include "RTSRegenerateHealthTask.h"
#include "Combat/Abilities/RTSGEHealthRegen.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/RTSGameplayLibrary.h"
#include "Libraries/RTSGameplayTagLibrary.h"
#include "Sound/SoundCue.h"

URTSCombatComponent::URTSCombatComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void URTSCombatComponent::UseAttack(int32 AttackIndex, AActor* Target)
{
	if (!Attacks.IsValidIndex(AttackIndex))
	{
		UE_LOG(LogRTS, Error, TEXT("There is no AttackData on Index %i"), AttackIndex)
		return;
	}

	const AActor* Owner = GetOwner();

	UE_LOG(LogRTS, Log, TEXT("URTSCombatComponent: Actor %s attacks %s."), *Owner->GetName(), *Target->GetName());

	LastTarget = Target;

	if (!TryActivateAbility(AbilitySpecs[AttackIndex]))
	{
		UE_LOG(LogRTS, Warning, TEXT("Could not activate %s"), *Attacks[AttackIndex].AbilityClass->GetName())
	}
}

float URTSCombatComponent::GetAttackRange(AActor* Target) const
{
	/* TODO concept for a per actor range check. Maybe based on damage-type or flying/grounded */
	if (!Attacks.Num())
	{
		return 0;
	}
	return Attacks[0].Range;
}

AActor* URTSCombatComponent::GetLastTarget() const
{
	return LastTarget.Get();
}

void URTSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet = Cast<URTSAttributeSet>(GetAttributeSet(URTSAttributeSet::StaticClass()));

	if (!IsValid(AttributeSet))
	{
		UE_LOG(LogRTS, Error, TEXT("No RTSAttributeSet provided"))
	}

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	for (const FRTSAttackData Attack : Attacks)
	{
		UGameplayAbility* GameplayAbility = Attack.AbilityClass->GetDefaultObject<UGameplayAbility>();
		FGameplayAbilitySpec Ability(
			GameplayAbility,
			0,
			0);

		AbilitySpecs.Emplace(GiveAbility(Ability));
	}

	if (bRegenerateHealth)
	{
		URTSRegenerateHealthTask* RegenerateHealthTask = URTSRegenerateHealthTask::NewTask<URTSRegenerateHealthTask>(this);
		RegenerateHealthTask->ReadyForActivation();
	}
}

void URTSCombatComponent::AddGameplayTags(FGameplayTagContainer& InOutTagContainer)
{
	InOutTagContainer.AddTag(URTSGameplayTagLibrary::Status_Changing_Alive());
	InOutTagContainer.AddTag(URTSGameplayTagLibrary::Status_Permanent_CanAttack());
}

float URTSCombatComponent::GetMaximumHealth() const
{
	if (!IsValid(AttributeSet))
	{
		return 0.0F;
	}

	return AttributeSet->GetMaxHealth();
}

float URTSCombatComponent::GetCurrentHealth() const
{
	if (!IsValid(AttributeSet))
	{
		return 0.0F;
	}

	return AttributeSet->GetHealth();
}

float URTSCombatComponent::GetHealthRegenerationRate() const
{
	return HealthRegenerationRate;
}

void URTSCombatComponent::SetHealthRegenerationRate(float Rate)
{
	HealthRegenerationRate = Rate;
}

void URTSCombatComponent::KillActor(AActor* DamageCauser) const
{
	AActor* Owner = GetOwner();

	UE_LOG(LogRTS, Log, TEXT("Actor %s has been killed."), *Owner->GetName());

	// Get owner before destruction.
	AController* OwningPlayer = Cast<AController>(Owner->GetOwner());

	// Remove Alive tag.
	URTSGameplayTagLibrary::RemoveGameplayTag(Owner, URTSGameplayTagLibrary::Status_Changing_Alive());

	// Notify listeners.
	OnKilled.Broadcast(Owner, OwningPlayer, DamageCauser);

	ARTSPlayerState* PlayerState = OwningPlayer->GetPlayerState<ARTSPlayerState>();
	if (PlayerState != nullptr)
	{
		PlayerState->Remove(Owner);
	}

	if (IsValid(DeathSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	}

	// Stop or destroy actor.
	switch (ActorDeathType)
	{
	case ERTSActorDeathType::DEATH_StopGameplay:
		URTSGameplayLibrary::StopGameplayFor(Owner);
		break;

	case ERTSActorDeathType::DEATH_Destroy:
		Owner->Destroy();
		break;
	default: ;
	}

	// Notify game mode.

	if (ARTSGameMode* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(GetWorld())); GameMode != nullptr)
	{
		GameMode->NotifyOnActorKilled(Owner, OwningPlayer);
	}
}

void URTSCombatComponent::NotifyOnHealthChanged(AActor* Actor, float OldHealth, float NewHealth, AActor* DamageCauser)
{
	// Notify listeners.
	OnHealthChanged.Broadcast(Actor, OldHealth, NewHealth, DamageCauser);

	if (NewHealth < OldHealth)
	{
		LastTimeDamageTaken = GetWorld()->GetRealTimeSeconds();
	}

	// Play sound.
	if (NewHealth <= 0.0f)
	{
		KillActor(DamageCauser);
	}
}
