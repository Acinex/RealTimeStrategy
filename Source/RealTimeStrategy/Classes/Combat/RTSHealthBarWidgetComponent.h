#pragma once

#include "CoreMinimal.h"

#include "UI/RTSActorWidgetComponent.h"

#include "RTSHealthBarWidgetComponent.generated.h"


class URTSHealthComponent;
class URTSCombatComponent;


/**
* Adds a health bar widget to the actor.
*/
UCLASS(ClassGroup="RTS", Category = "RTS", Blueprintable)
class REALTIMESTRATEGY_API URTSHealthBarWidgetComponent : public URTSActorWidgetComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    /** Event when the current health of the actor has changed. */
    UFUNCTION(BlueprintImplementableEvent, Category = "RTS")
    void UpdateHealthBar(float HealthPercentage);

private:
	UPROPERTY()
	URTSCombatComponent* CombatComponent;
	UPROPERTY()
	URTSHealthComponent* HealthComponent;

	UFUNCTION()
	virtual void OnHealthChanged(AActor* Actor, float OldHealth, float NewHealth, AActor* DamageCauser);
};
