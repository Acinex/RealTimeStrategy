#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "RTSSelfRegisteringActorComponent.h"
#include "Vision/RTSVisionState.h"

#include "RTSVisibleComponent.generated.h"


class AActor;
class AController;

class URTSVisibleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRTSVisibleComponentVisionStageChangedSignature, URTSVisibleComponent*, Component, ERTSVisionState, VisionState);

/** Allows the actor to be hidden by fog of war or other effects. */
UCLASS(ClassGroup="RTS", Category="RTS", meta=(BlueprintSpawnableComponent))
class REALTIMESTRATEGY_API URTSVisibleComponent : public URTSSelfRegisteringActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/** Server. Gets the vision state of the actor for the specified player. */
	UFUNCTION(BlueprintCallable)
	ERTSVisionState GetVisionStateForPlayer(AController* Controller) const;

	/** Client. Whether the actor is visible due to vision for the local player. */
	UFUNCTION(BlueprintPure)
	bool IsVisibleForLocalClient() const;

	/** Server. Whether the actor is visible due to vision for the specified player. */
	UFUNCTION(BlueprintCallable)
	bool IsVisibleForPlayer(AController* Controller) const;

	/** Client. Override the visibility of the actor, forcing it to be visible or hidden because of the specified reason. */
	UFUNCTION(BlueprintCallable)
	void SetClientHideReason(const FGameplayTag& HideReason, bool bHidden);

	/** Client. Remembers whether the actor is hidden due to vision for the local player. */
	void SetClientVisionState(ERTSVisionState InVisionState);

	/** Server. Sets the vision state of the actor for the specified player. */
	void SetVisionStateForPlayer(AController* Controller, ERTSVisionState InVisionState);

	/** Event when the current vision stage was changed. */
	UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSVisibleComponentVisionStageChangedSignature OnVisionStateChanged;

private:
	/** Prevent the actor from ever being hidden again after seen once (e.g. buildings). */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	bool bDontHideAfterSeen = false;

	/** Whether the actor and all children are currently visible, or not. */
	bool bVisible;

	/** Server. Whether the actor is hidden due to vision, by player. */
	UPROPERTY()
	TMap<AController*, ERTSVisionState> VisionStates;

	/** Server. Players that have ever seen the actor. */
	UPROPERTY()
	TMap<AController*, bool> bWasEverSeen;

	/** Client. Whether the actor is hidden due to vision for the local player. */
	ERTSVisionState ClientVisionState;

	/** Client. Whether the local player has ever seen the actor. */
	bool bClientWasEverSeen;

	/** Client. Reasons for which the actor is currently hidden. */
	FGameplayTagContainer HideReasonTags;

	UFUNCTION()
	void OnContainerChanged(AActor* Actor, AActor* NewContainer);

	/** Client. Shows or hides the actor locally, based on all effects that can hide actors (e.g. containers, fog of war). */
	void UpdateClientHiddenFlag();

	/** Client. Whether the actor is owned by a player that is in the same team as the local player. */
	bool IsSameTeamAsLocalClient() const;

	/** Client. Sets the actor and all of its components visible or invisible. */
	void SetVisible(bool bInVisible);
};
